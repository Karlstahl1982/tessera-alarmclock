#include "alarmclock.h"
#include <cstdio>

namespace esphome::alarmclock {

namespace ui = tessera::ui;
using tessera::Font;

class NextAlarmTile : public tessera::Tile {
 public:
  explicit NextAlarmTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    tile_ = c.tile;
    width_ = c.width;
    height_ = c.height;
    headline_ = ui::label(c.parent, Font::TITLE, theme::INK);
    detail_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    lv_obj_set_width(headline_, width_);
    lv_obj_set_width(detail_, width_);
    lv_obj_set_style_text_align(headline_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(detail_, LV_TEXT_ALIGN_CENTER, 0);
    const int upper = ui::line_height(Font::TITLE);
    const int lower = ui::line_height(Font::BODY);
    const int y = (height_ - upper - lower) / 2;
    lv_obj_set_pos(headline_, 0, y);
    lv_obj_set_height(headline_, upper);
    lv_obj_set_pos(detail_, 0, y + upper);
    lv_obj_set_height(detail_, lower);
    ui::set_text(headline_, plugin_->text("title"));
    ui::set_text(detail_, plugin_->text("none"));
  }

  void on_theme() override {
    ui::set_color(headline_, theme::INK);
    ui::set_color(detail_, theme::MUTED);
  }

  void on_tap() override {
    tessera::open_card(plugin_->plugin_id(), "overview", "", tile_);
  }

 private:
  const tessera::Plugin *plugin_;
  int tile_{-1};
  int width_{0};
  int height_{0};
  lv_obj_t *headline_{nullptr};
  lv_obj_t *detail_{nullptr};
};

class AlarmOverview : public tessera::Card {
 public:
  explicit AlarmOverview(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void open(const tessera::CardContext &c) override {
    const int width = c.width;
    const int height = c.height;
    const int body_h = ui::line_height(Font::BODY);
    const int gap = 5;
    const int total = body_h * 6 + gap * 5;
    const int top = total < height ? (height - total) / 2 : 0;
    for (int i = 0; i < 5; ++i) {
      rows_[i] = ui::label(c.parent, Font::BODY, theme::INK);
      lv_obj_set_width(rows_[i], width);
      lv_obj_set_height(rows_[i], body_h);
      lv_obj_set_pos(rows_[i], 0, top + i * (body_h + gap));
      lv_obj_set_style_text_align(rows_[i], LV_TEXT_ALIGN_CENTER, 0);
      char line[64];
      snprintf(line, sizeof(line), "%s %d  --:--", plugin_->text("alarm").c_str(), i + 1);
      ui::set_text(rows_[i], line);
    }
    foot_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    lv_obj_set_width(foot_, width);
    lv_obj_set_height(foot_, body_h);
    lv_obj_set_style_text_align(foot_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(foot_, 0, top + 5 * (body_h + gap));
    ui::set_text(foot_, plugin_->text("prototype"));
  }

  void on_theme() override {
    for (auto *row : rows_) if (row) ui::set_color(row, theme::INK);
    if (foot_) ui::set_color(foot_, theme::MUTED);
  }

 private:
  const tessera::Plugin *plugin_;
  lv_obj_t *rows_[5]{};
  lv_obj_t *foot_{nullptr};
};

void AlarmClock::setup() {
  add_tile("next_alarm", [this]() { return new NextAlarmTile(this); });
  add_card("overview", [this]() { return new AlarmOverview(this); }, true);
}

}  // namespace esphome::alarmclock
