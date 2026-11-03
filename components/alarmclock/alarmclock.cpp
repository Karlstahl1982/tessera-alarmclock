#include "alarmclock.h"
#include <cstdio>

namespace esphome::alarmclock {

namespace ui = tessera::ui;
using tessera::Font;

static void time_text(const AlarmClock *a, char *buffer, size_t size) {
  std::snprintf(buffer, size, "%02d:%02d", a->hour(), a->minute());
}

class NextAlarmTile : public tessera::Tile {
 public:
  explicit NextAlarmTile(AlarmClock *plugin) : plugin_(plugin) {}
  void create(const tessera::TileContext &c) override {
    tile_ = c.tile;
    headline_ = ui::label(c.parent, Font::TITLE, theme::INK);
    detail_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    lv_obj_set_width(headline_, c.width);
    lv_obj_set_width(detail_, c.width);
    lv_obj_set_style_text_align(headline_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(detail_, LV_TEXT_ALIGN_CENTER, 0);
    const int upper = ui::line_height(Font::TITLE);
    const int lower = ui::line_height(Font::BODY);
    const int y = (c.height - upper - lower) / 2;
    lv_obj_set_pos(headline_, 0, y);
    lv_obj_set_height(headline_, upper);
    lv_obj_set_pos(detail_, 0, y + upper);
    lv_obj_set_height(detail_, lower);
    on_tick(0);
  }
  void on_tick(uint32_t) override {
    ui::set_text(headline_, plugin_->text("title"));
    if (plugin_->ringing()) {
      ui::set_text(detail_, plugin_->text("ringing"));
    } else if (plugin_->enabled()) {
      char buffer[32];
      time_text(plugin_, buffer, sizeof(buffer));
      ui::set_text(detail_, buffer);
    } else {
      ui::set_text(detail_, plugin_->text("none"));
    }
  }
  void on_theme() override {
    ui::set_color(headline_, theme::INK);
    ui::set_color(detail_, theme::MUTED);
  }
  void on_tap() override {
    tessera::open_card(plugin_->plugin_id(), "overview", "", tile_);
  }
 private:
  AlarmClock *plugin_;
  int tile_{-1};
  lv_obj_t *headline_{nullptr}, *detail_{nullptr};
};

class AlarmOverview : public tessera::Card {
 public:
  explicit AlarmOverview(AlarmClock *plugin) : plugin_(plugin) {}
  void open(const tessera::CardContext &c) override {
    width_ = c.width;
    const int body_h = ui::line_height(Font::BODY);
    const int gap = 7;
    const int total = body_h * 6 + gap * 5;
    top_ = total < c.height ? (c.height - total) / 2 : 0;
    stride_ = body_h + gap;
    for (int i = 0; i < 5; ++i) {
      rows_[i] = ui::label(c.parent, Font::BODY, theme::INK);
      lv_obj_set_width(rows_[i], width_);
      lv_obj_set_height(rows_[i], body_h);
      lv_obj_set_pos(rows_[i], 0, top_ + i * stride_);
      lv_obj_set_style_text_align(rows_[i], LV_TEXT_ALIGN_CENTER, 0);
    }
    foot_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    lv_obj_set_width(foot_, width_);
    lv_obj_set_height(foot_, body_h);
    lv_obj_set_style_text_align(foot_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(foot_, 0, top_ + 5 * stride_);
    on_tick(0);
  }
  void on_tick(uint32_t) override {
    char line[100];
    char time[12];
    time_text(plugin_, time, sizeof(time));
    if (edit_) {
      std::snprintf(line, sizeof(line), "%s 1: %s", plugin_->text("alarm"), time);
      ui::set_text(rows_[0], line);
      std::snprintf(line, sizeof(line), "-     %s: %02d     +", plugin_->text("hour"), plugin_->hour());
      ui::set_text(rows_[1], line);
      std::snprintf(line, sizeof(line), "-     %s: %02d     +", plugin_->text("minute"), plugin_->minute());
      ui::set_text(rows_[2], line);
      std::snprintf(line, sizeof(line), "%s: %s", plugin_->text("enabled"),
                    plugin_->enabled() ? plugin_->text("on") : plugin_->text("off"));
      ui::set_text(rows_[3], line);
      ui::set_text(rows_[4], plugin_->text("back"));
      ui::set_text(foot_, plugin_->text("edit_hint"));
    } else {
      std::snprintf(line, sizeof(line), "%s 1   %s  (%s)", plugin_->text("alarm"), time,
                    plugin_->enabled() ? plugin_->text("on") : plugin_->text("off"));
      ui::set_text(rows_[0], line);
      for (int i = 1; i < 5; ++i) {
        std::snprintf(line, sizeof(line), "%s %d   --:--", plugin_->text("alarm"), i + 1);
        ui::set_text(rows_[i], line);
      }
      ui::set_text(foot_, plugin_->text(plugin_->ringing() ? "tap_stop" : "tap_edit"));
    }
  }
  void on_tap() override {
    if (plugin_->ringing()) {
      plugin_->stop_alarm();
      on_tick(0);
      return;
    }
    const auto point = tessera::tap_point();
    if (point.y < top_) return;
    const int row = (point.y - top_) / stride_;
    if (!edit_) {
      if (row == 0) edit_ = true;
    } else if (row == 1 || row == 2) {
      // Left side decreases, right side increases; wrap around midnight/59.
      const int delta = point.x < width_ / 2 ? -1 : 1;
      if (row == 1) plugin_->adjust_hour(delta);
      else plugin_->adjust_minute(delta);
    } else if (row == 3) {
      plugin_->toggle();
    } else if (row == 4) {
      edit_ = false;
    }
    on_tick(0);
  }
  void on_theme() override {
    for (auto *row : rows_) if (row) ui::set_color(row, theme::INK);
    if (foot_) ui::set_color(foot_, theme::MUTED);
  }
 private:
  AlarmClock *plugin_;
  lv_obj_t *rows_[5]{};
  lv_obj_t *foot_{nullptr};
  int width_{0}, top_{0}, stride_{0};
  bool edit_{false};
};

int AlarmClock::hour() const {
  return alarm_time_ && alarm_time_->has_state() ? alarm_time_->hour : 7;
}
int AlarmClock::minute() const {
  return alarm_time_ && alarm_time_->has_state() ? alarm_time_->minute : 0;
}
void AlarmClock::adjust_hour(int delta) {
  if (!alarm_time_) return;
  const int h = (hour() + delta + 24) % 24;
  auto call = alarm_time_->make_call();
  call.set_time(static_cast<uint8_t>(h), static_cast<uint8_t>(minute()), 0);
  call.perform();
  tessera::refresh();
}
void AlarmClock::adjust_minute(int delta) {
  if (!alarm_time_) return;
  const int m = (minute() + delta + 60) % 60;
  auto call = alarm_time_->make_call();
  call.set_time(static_cast<uint8_t>(hour()), static_cast<uint8_t>(m), 0);
  call.perform();
  tessera::refresh();
}
bool AlarmClock::enabled() const { return enabled_ && enabled_->state; }
void AlarmClock::toggle() {
  if (!enabled_) return;
  if (enabled_->state) enabled_->turn_off(); else enabled_->turn_on();
  tessera::refresh();
}
void AlarmClock::start_alarm_(bool test) {
  if (!tone_) return;
  ringing_ = true;
  test_ = test;
  alarm_started_ms_ = millis();
  last_play_ms_ = alarm_started_ms_;
  tone_->stop();
  tone_->play("alarm:d=8,o=5,b=105:c,e,g,4c6");
  tessera::wake(this);  // API 0.16: wake a dark display for the alarm
  tessera::refresh();
}
void AlarmClock::start_test() { start_alarm_(true); }
void AlarmClock::stop_alarm() {
  if (!ringing_) return;
  ringing_ = false;
  if (tone_) tone_->stop();
  tessera::refresh();
}
void AlarmClock::on_interval(uint32_t now_ms) {
  if (ringing_) {
    const uint32_t duration = test_ ? 15000u : 300000u;
    if (static_cast<uint32_t>(now_ms - alarm_started_ms_) >= duration) {
      stop_alarm();
    } else if (tone_ && !tone_->is_playing() && static_cast<uint32_t>(now_ms - last_play_ms_) >= 800u) {
      tone_->play("alarm:d=8,o=5,b=105:c,e,g,4c6");
      last_play_ms_ = now_ms;
    }
  }
  const uint32_t epoch = tessera::epoch();
  if (!epoch) return;  // RTC has not been set yet: never guess a time
  const uint32_t minute_index = epoch / 60;
  if (minute_index == last_seen_minute_) return;
  last_seen_minute_ = minute_index;
  if (!enabled() || ringing_) return;
  const auto local = tessera::local_time(epoch);
  if (local.hour != hour() || local.minute != minute()) return;
  enabled_->turn_off();  // single-shot alarm: never ring the next day without arming it again
  start_alarm_(false);
}
bool AlarmClock::settings(tessera::SettingsPage &page) {
  page.icon = "\U000F0020";
  page.toggle(text("enabled"), [this] { return enabled(); },
              [this](bool on) { if (on) enabled_->turn_on(); else enabled_->turn_off(); });
  page.card(text("open_card"), "\U000F0020", "overview");
  return true;
}
void AlarmClock::setup() {
  add_tile("next_alarm", [this]() { return new NextAlarmTile(this); });
  add_card("overview", [this]() { return new AlarmOverview(this); }, true);
}

}  // namespace esphome::alarmclock
