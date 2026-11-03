#include "alarmclock.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>

namespace esphome::alarmclock {
namespace ui = tessera::ui;
using tessera::Font;

namespace {
constexpr const char *DAYS[] = {"Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"};
constexpr const char *MODES[] = {"Next", "Date", "Weekly"};
constexpr const char *TUNES[] = {"Classic", "Gentle", "Digital", "Sunrise", "Urgent"};
// Five short original sequences; RTTTL playback loops until the alarm ends.
constexpr const char *SONGS[] = {
    "classic:d=8,o=5,b=110:c,p,c,p,c,p,4c",
    "gentle:d=8,o=5,b=88:c,e,g,4c6,p,g,e,4c",
    "digital:d=16,o=5,b=135:g,c6,g,c6,p,g,c6,8g",
    "sunrise:d=8,o=5,b=85:c,d,e,g,4c6,p,g,e,c",
    "urgent:d=16,o=6,b=180:a,a,p,a,a,p,g,g,p,a,a"};
int clamp_int(int value, int lo, int hi) { return std::max(lo, std::min(hi, value)); }
int days_in_month(int year, int month) {
  static constexpr int DAYS_IN_MONTH[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) return 29;
  return month >= 1 && month <= 12 ? DAYS_IN_MONTH[month] : 31;
}
std::string list_numbers(int from, int to, int digits) {
  std::string result;
  char buffer[16];
  for (int i = from; i <= to; ++i) {
    if (!result.empty()) result += '\n';
    std::snprintf(buffer, sizeof(buffer), digits == 4 ? "%04d" : "%02d", i);
    result += buffer;
  }
  return result;
}
void place_label(lv_obj_t *label, int x, int y, int width, int height, const char *value) {
  lv_obj_set_pos(label, x, y);
  lv_obj_set_size(label, width, height);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  ui::set_text(label, value);
}
void toggle_hidden(lv_obj_t *obj, bool visible) {
  if (!obj) return;
  if (visible) lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
  else lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}
void color_roller(lv_obj_t *roller) {
  lv_obj_set_style_bg_color(roller, ui::color(theme::CARD), LV_PART_MAIN);
  lv_obj_set_style_text_color(roller, ui::color(theme::INK), LV_PART_MAIN);
  lv_obj_set_style_bg_color(roller, ui::color(theme::ACCENT), LV_PART_SELECTED);
  lv_obj_set_style_text_color(roller, ui::color(theme::ON_ACCENT), LV_PART_SELECTED);
  lv_obj_set_style_text_font(roller, ui::font(Font::VALUE), LV_PART_MAIN);
  lv_obj_set_style_border_width(roller, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(roller, ui::px(12), LV_PART_MAIN);
}
}  // namespace

class NextAlarmTile final : public tessera::Tile {
 public:
  explicit NextAlarmTile(AlarmClock *p) : p_(p) {}
  void create(const tessera::TileContext &c) override {
    index_ = c.tile;
    name_ = ui::label(c.parent, Font::TITLE, theme::INK);
    value_ = ui::label(c.parent, Font::VALUE, theme::INK);
    const int name_h = ui::line_height(Font::TITLE);
    const int value_h = ui::line_height(Font::VALUE);
    const int y = std::max(0, (c.height - name_h - value_h - ui::px(4)) / 2);
    place_label(name_, 0, y, c.width, name_h, p_->text("tile_title"));
    place_label(value_, 0, y + name_h + ui::px(4), c.width, value_h, "--:--");
    on_tick(0);
  }
  void on_tick(uint32_t) override {
    if (p_->any_ringing()) { ui::set_text(value_, p_->text("ringing")); return; }
    int n = p_->next_alarm();
    if (n < 0) { ui::set_text(value_, "--:--"); return; }
    char buffer[20];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", p_->hour(n), p_->minute(n));
    ui::set_text(value_, buffer);
  }
  void on_theme() override {
    ui::set_color(name_, theme::INK);
    ui::set_color(value_, theme::INK);
  }
  void on_tap() override { tessera::open_card(p_->plugin_id(), "overview", "", index_); }
 private:
  AlarmClock *p_{};
  int index_{-1};
  lv_obj_t *name_{}, *value_{};
};

class AlarmOverview final : public tessera::Card {
 public:
  explicit AlarmOverview(AlarmClock *p) : p_(p) {}
  void open(const tessera::CardContext &c) override {
    w_ = c.width;
    h_ = c.height;
    title_ = ui::label(c.parent, Font::TITLE, theme::INK);
    rows_[0] = ui::label(c.parent, Font::BODY_LARGE, theme::INK);
    for (int i = 1; i < 7; ++i) rows_[i] = ui::label(c.parent, Font::BODY, theme::INK);
    for (int i = 0; i < 7; ++i) days_[i] = ui::label(c.parent, Font::BODY, theme::INK);
    footer_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    hour_ = lv_roller_create(c.parent);
    minute_ = lv_roller_create(c.parent);
    day_ = lv_roller_create(c.parent);
    month_ = lv_roller_create(c.parent);
    year_ = lv_roller_create(c.parent);
    // Native LVGL rollers handle swipes; the Save / Options rows use Tessera's filtered on_tap.
    const auto hours = list_numbers(0,23,2);
    const auto minutes = list_numbers(0,59,2);
    const auto dates = list_numbers(1,31,2);
    const auto months = list_numbers(1,12,2);
    const auto years = list_numbers(2025,2045,4);
    lv_roller_set_options(hour_, hours.c_str(), LV_ROLLER_MODE_INFINITE);
    lv_roller_set_options(minute_, minutes.c_str(), LV_ROLLER_MODE_INFINITE);
    lv_roller_set_options(day_, dates.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_options(month_, months.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_options(year_, years.c_str(), LV_ROLLER_MODE_NORMAL);
    for (auto *r : {hour_, minute_, day_, month_, year_}) {
      lv_roller_set_visible_row_count(r, 3);
      color_roller(r);
    }
    render_();
  }
  void on_tick(uint32_t) override { render_(); }
  void on_theme() override {
    ui::set_color(title_, theme::INK);
    for (auto *l : rows_) ui::set_color(l, theme::INK);
    for (auto *l : days_) ui::set_color(l, theme::INK);
    ui::set_color(footer_, theme::MUTED);
    for (auto *r : {hour_,minute_,day_,month_,year_}) color_roller(r);
  }
  bool on_back() override {
    if (screen_ != OVERVIEW) { screen_ = OVERVIEW; render_(); return true; }
    return false;
  }
  void on_tap() override {
    const auto pt = tessera::tap_point();
    if (pt.x < 0 || pt.y < 0) return;
    const int x = pt.x, y = pt.y;
    if (screen_ == OVERVIEW) {
      const int top = ui::px(32);
      const int row_height = std::max(ui::mm(9), (h_ - top - ui::px(25)) / 5);
      const int index = (y - top) / row_height;
      if (index >= 0 && index < 5) {
        selected_ = index;
        if (x > w_ * 4 / 5) p_->set_active(index, !p_->enabled(index));
        else begin_time_();
      }
    } else if (screen_ == TIME) {
      if (y > h_ - ui::mm(14)) { save_time_(); screen_ = OVERVIEW; }
      else if (y > h_ - ui::mm(29)) { save_time_(); screen_ = DETAILS; }
    } else if (screen_ == DETAILS) {
      details_tap_(x,y);
    } else if (screen_ == DATE) {
      if (y > h_ - ui::mm(14)) { save_date_(); screen_ = DETAILS; }
    }
    render_();
  }
 private:
  enum Screen { OVERVIEW, TIME, DETAILS, DATE } screen_{OVERVIEW};
  void begin_time_() {
    lv_roller_set_selected(hour_, p_->hour(selected_), LV_ANIM_OFF);
    lv_roller_set_selected(minute_, p_->minute(selected_), LV_ANIM_OFF);
    screen_ = TIME;
  }
  void save_time_() {
    p_->set_alarm_time(selected_, lv_roller_get_selected(hour_), lv_roller_get_selected(minute_));
  }
  void begin_date_() {
    lv_roller_set_selected(day_, clamp_int(p_->day(selected_),1,31)-1, LV_ANIM_OFF);
    lv_roller_set_selected(month_, clamp_int(p_->month(selected_),1,12)-1, LV_ANIM_OFF);
    lv_roller_set_selected(year_, clamp_int(p_->year(selected_),2025,2045)-2025, LV_ANIM_OFF);
    screen_ = DATE;
  }
  void save_date_() {
    const int y = 2025 + lv_roller_get_selected(year_);
    const int m = 1 + lv_roller_get_selected(month_);
    const int d = std::min(1 + static_cast<int>(lv_roller_get_selected(day_)), days_in_month(y,m));
    p_->set_alarm_date(selected_,y,m,d);
  }
  void details_tap_(int x, int y) {
    const int top = ui::px(30);
    const int lh = std::max(ui::mm(8), (h_-top-ui::px(5))/8);
    int row = (y-top) / lh;
    switch (row) {
      case 0: p_->set_alarm_mode(selected_, (p_->mode(selected_)+1)%3); break;
      case 1: if (p_->mode(selected_) == 1) begin_date_(); break;
      case 2: if (p_->mode(selected_) == 2) {
          int day = clamp_int(x * 7 / std::max(1,w_),0,6);
          p_->set_weekday(selected_,day,!p_->weekday_enabled(selected_,day));
        } break;
      case 3: p_->set_alarm_tone(selected_,(p_->tune(selected_)+1)%5); break;
      case 4: p_->set_alarm_volume(selected_,p_->volume(selected_) >= 100 ? 10 : p_->volume(selected_)+10); break;
      case 5: p_->set_snooze_minutes(selected_,p_->snooze_minutes(selected_)==5 ? 10 : 5); break;
      case 6: p_->set_active(selected_,!p_->enabled(selected_)); break;
      case 7: screen_=OVERVIEW; break;
    }
  }
  void render_() {
    if (!title_) return;
    for (auto *r : {hour_,minute_,day_,month_,year_}) toggle_hidden(r,false);
    for (auto *row : rows_) toggle_hidden(row,false);
    for (auto *day : days_) toggle_hidden(day,false);
    toggle_hidden(footer_,true);
    toggle_hidden(title_,true);
    char buf[110];
    const int lh = ui::line_height(Font::BODY);
    if (screen_ == OVERVIEW) {
      ui::set_text(title_, p_->text("list_title"));
      lv_obj_set_pos(title_, 0, 0);
      lv_obj_set_width(title_, w_);
      const int top = ui::px(32);
      const int row_h = std::max(ui::mm(9),(h_-top-ui::px(25))/5);
      for (int i = 0; i < 5; ++i) {
        std::snprintf(buf,sizeof(buf),"%d    %02d:%02d   %s",i+1,p_->hour(i),p_->minute(i),p_->enabled(i)?p_->text("on"):p_->text("off"));
        place_label(rows_[i], 0, top+i*row_h, w_, row_h, buf);
      }
      place_label(footer_,0,h_-lh,w_,lh,p_->time_valid()?p_->text("list_hint"):p_->text("clock_missing"));
    } else if (screen_ == TIME) {
      std::snprintf(buf,sizeof(buf),"%s %d",p_->text("alarm"),selected_+1);
      place_label(title_,0,0,w_,ui::line_height(Font::TITLE),buf);
      const int rw = std::min(ui::px(132), (w_-ui::px(30))/2);
      const int ry = std::max(ui::px(42), (h_-ui::px(210))/2);
      lv_obj_set_size(hour_,rw,ui::px(166));
      lv_obj_set_size(minute_,rw,ui::px(166));
      lv_obj_set_pos(hour_,w_/2-rw-ui::px(5),ry);
      lv_obj_set_pos(minute_,w_/2+ui::px(5),ry);
      toggle_hidden(hour_,true);
      toggle_hidden(minute_,true);
      place_label(rows_[0],0,h_-ui::mm(27),w_,ui::mm(12),p_->text("options"));
      place_label(rows_[1],0,h_-ui::mm(13),w_,ui::mm(12),p_->text("save"));
    } else if (screen_ == DATE) {
      std::snprintf(buf,sizeof(buf),"%s %d",p_->text("date"),selected_+1);
      place_label(title_,0,0,w_,ui::line_height(Font::TITLE),buf);
      int rw=std::max(40,(w_-ui::px(24))/3);
      const int ry=std::max(ui::px(40),(h_-ui::px(200))/2);
      lv_obj_set_size(day_,rw,ui::px(150));
      lv_obj_set_size(month_,rw,ui::px(150));
      lv_obj_set_size(year_,rw,ui::px(150));
      lv_obj_set_pos(day_,0,ry);
      lv_obj_set_pos(month_,rw+ui::px(7),ry);
      lv_obj_set_pos(year_,2*(rw+ui::px(7)),ry);
      toggle_hidden(day_,true); toggle_hidden(month_,true); toggle_hidden(year_,true);
      place_label(rows_[0],0,h_-ui::mm(13),w_,ui::mm(12),p_->text("save"));
    } else if (screen_ == DETAILS) {
      const int top = ui::px(30);
      const int rh = std::max(ui::mm(8),(h_-top-ui::px(5))/8);
      std::snprintf(buf,sizeof(buf),"%s %d",p_->text("options"),selected_+1);
      place_label(title_,0,0,w_,ui::line_height(Font::TITLE),buf);
      std::snprintf(buf,sizeof(buf),"%s: %s",p_->text("mode"),p_->mode_label(selected_));
      place_label(rows_[0],0,top+0*rh,w_,rh,buf);
      std::snprintf(buf,sizeof(buf),"%s: %04d-%02d-%02d",p_->text("date"),p_->year(selected_),p_->month(selected_),p_->day(selected_));
      place_label(rows_[1],0,top+1*rh,w_,rh,buf);
      if (p_->mode(selected_) == 2) {
        toggle_hidden(rows_[2], false);
        const int cw=w_/7;
        for (int i=0;i<7;++i) {
          place_label(days_[i],i*cw,top+2*rh,cw,rh,DAYS[i]);
          ui::set_color(days_[i],p_->weekday_enabled(selected_,i)?theme::ACCENT:theme::MUTED);
          toggle_hidden(days_[i],true);
        }
      } else {
        place_label(rows_[2],0,top+2*rh,w_,rh,p_->text("weekdays_off"));
      }
      std::snprintf(buf,sizeof(buf),"%s: %s",p_->text("tone"),p_->tone_label(selected_));
      place_label(rows_[3],0,top+3*rh,w_,rh,buf);
      std::snprintf(buf,sizeof(buf),"%s: %d%%",p_->text("volume"),p_->volume(selected_));
      place_label(rows_[4],0,top+4*rh,w_,rh,buf);
      std::snprintf(buf,sizeof(buf),"%s: %d min",p_->text("snooze"),p_->snooze_minutes(selected_));
      place_label(rows_[5],0,top+5*rh,w_,rh,buf);
      std::snprintf(buf,sizeof(buf),"%s: %s",p_->text("enabled"),p_->enabled(selected_)?p_->text("on"):p_->text("off"));
      place_label(rows_[6],0,top+6*rh,w_,rh,buf);
      place_label(footer_,0,top+7*rh,w_,rh,p_->text("back"));
    }
  }
  AlarmClock *p_{};
  int selected_{0}, w_{0}, h_{0};
  lv_obj_t *title_{}, *rows_[7]{}, *days_[7]{}, *footer_{};
  lv_obj_t *hour_{}, *minute_{}, *day_{}, *month_{}, *year_{};
};

class AlarmPopup final : public tessera::Popup {
 public:
  explicit AlarmPopup(AlarmClock *p) : p_(p) {}
  void open(const tessera::PopupContext &c) override {
    w_=c.width; h_=c.height;
    title_=ui::label(c.parent,Font::HEADLINE,theme::INK);
    clock_=ui::label(c.parent,Font::VALUE,theme::INK);
    status_=ui::label(c.parent,Font::BODY,theme::MUTED);
    stop_=ui::label(c.parent,Font::BODY_LARGE,theme::INK);
    snooze_=ui::label(c.parent,Font::BODY_LARGE,theme::INK);
    on_tick(0);
  }
  void on_tick(uint32_t) override {
    int i=p_->first_ringing();
    if (i<0) { tessera::close_popup(p_); return; }
    char b[70];
    std::snprintf(b,sizeof(b),"%s %d",p_->text("alarm"),i+1);
    place_label(title_,0,0,w_*3/4,ui::line_height(Font::HEADLINE),b);
    std::snprintf(b,sizeof(b),"%02d:%02d",p_->hour(i),p_->minute(i));
    place_label(clock_,0,ui::px(62),w_,ui::px(70),b);
    std::snprintf(b,sizeof(b),"%s: %d/3",p_->text("snooze"),p_->snoozes_used(i));
    place_label(status_,0,ui::px(138),w_,ui::line_height(Font::BODY),b);
    place_label(snooze_,0,ui::px(190),w_/2,ui::mm(13),p_->snoozes_used(i)<3?p_->text("snooze"):"--");
    place_label(stop_,w_/2,ui::px(190),w_/2,ui::mm(13),p_->text("stop"));
  }
  void on_theme() override {
    ui::set_color(title_,theme::INK); ui::set_color(clock_,theme::INK);
    ui::set_color(status_,theme::MUTED); ui::set_color(stop_,theme::INK);
    ui::set_color(snooze_,theme::INK);
  }
  void on_tap() override {
    auto pt=tessera::tap_point();
    if (pt.y<ui::px(175)) return;
    int i=p_->first_ringing();
    if (i<0) return;
    if (pt.x < w_/2) p_->snooze_alarm(i);
    else p_->stop_alarm(i);
    if (!p_->any_ringing()) tessera::close_popup(p_);
    else on_tick(0);
  }
 private:
  AlarmClock *p_{};
  int w_{0},h_{0};
  lv_obj_t *title_{},*clock_{},*status_{},*stop_{},*snooze_{};
};

int AlarmClock::hour(int i) const { return a_[i].time && a_[i].time->has_state() ? a_[i].time->hour : 7; }
int AlarmClock::minute(int i) const { return a_[i].time && a_[i].time->has_state() ? a_[i].time->minute : 0; }
int AlarmClock::year(int i) const { return a_[i].date && a_[i].date->has_state() ? a_[i].date->year : 2026; }
int AlarmClock::month(int i) const { return a_[i].date && a_[i].date->has_state() ? a_[i].date->month : 11; }
int AlarmClock::day(int i) const { return a_[i].date && a_[i].date->has_state() ? a_[i].date->day : 4; }
bool AlarmClock::enabled(int i) const { return a_[i].enabled && a_[i].enabled->state; }
int AlarmClock::mode(int i) const {
  if (!a_[i].mode) return 0;
  const char *m=a_[i].mode->current_option().c_str();
  for (int k=0;k<3;++k) if (std::strcmp(m,MODES[k])==0) return k;
  return 0;
}
int AlarmClock::tune(int i) const {
  if (!a_[i].tone) return 0;
  const char *t=a_[i].tone->current_option().c_str();
  for (int k=0;k<5;++k) if (std::strcmp(t,TUNES[k])==0) return k;
  return 0;
}
int AlarmClock::snooze_minutes(int i) const {
  return a_[i].snooze && std::strcmp(a_[i].snooze->current_option().c_str(),"10")==0 ? 10 : 5;
}
int AlarmClock::volume(int i) const { return a_[i].volume ? clamp_int(static_cast<int>(a_[i].volume->state),10,100) : 60; }
const char *AlarmClock::mode_label(int i) const { static constexpr const char *K[]={"mode_next","mode_date","mode_weekly"}; return this->text(K[mode(i)]); }
const char *AlarmClock::tone_label(int i) const { static constexpr const char *K[]={"tone_classic","tone_gentle","tone_digital","tone_sunrise","tone_urgent"}; return this->text(K[tune(i)]); }
uint8_t AlarmClock::weekdays(int i) const {
  if (!a_[i].days) return 31;
  const std::string &v=a_[i].days->state;
  uint8_t mask=0;
  for (int d=0;d<7;++d) if (v.find(DAYS[d])!=std::string::npos) mask |= 1u<<d;
  return mask;
}
bool AlarmClock::weekday_enabled(int i, int d) const { return d>=0 && d<7 && (weekdays(i)&(1u<<d)); }
void AlarmClock::set_alarm_time(int i,int h,int m) {
  if (!a_[i].time) return;
  auto call=a_[i].time->make_call();
  call.set_time(static_cast<uint8_t>(clamp_int(h,0,23)),static_cast<uint8_t>(clamp_int(m,0,59)),0);
  call.perform(); tessera::refresh();
}
void AlarmClock::set_alarm_date(int i,int y,int m,int d) {
  if (!a_[i].date) return;
  y=clamp_int(y,2025,2045); m=clamp_int(m,1,12); d=clamp_int(d,1,days_in_month(y,m));
  auto call=a_[i].date->make_call(); call.set_date(y,m,d); call.perform(); tessera::refresh();
}
void AlarmClock::set_alarm_mode(int i,int m) {
  if (!a_[i].mode) return;
  auto call=a_[i].mode->make_call(); call.set_option(MODES[clamp_int(m,0,2)]); call.perform(); tessera::refresh();
}
void AlarmClock::set_alarm_tone(int i,int t) {
  if (!a_[i].tone) return;
  auto call=a_[i].tone->make_call(); call.set_option(TUNES[clamp_int(t,0,4)]); call.perform(); tessera::refresh();
}
void AlarmClock::set_alarm_volume(int i,int v) {
  if (!a_[i].volume) return;
  auto call=a_[i].volume->make_call(); call.set_value(clamp_int(v,10,100)); call.perform(); tessera::refresh();
}
void AlarmClock::set_snooze_minutes(int i,int mins) {
  if (!a_[i].snooze) return;
  auto call=a_[i].snooze->make_call(); call.set_option(mins==10?"10":"5"); call.perform(); tessera::refresh();
}
void AlarmClock::set_weekday(int i,int d,bool on) {
  if (!a_[i].days || d<0 || d>6) return;
  uint8_t mask=weekdays(i);
  if (on) mask|=(1u<<d); else mask&=~(1u<<d);
  std::string value;
  for (int k=0;k<7;++k) if (mask&(1u<<k)) { if (!value.empty()) value+=','; value+=DAYS[k]; }
  auto call=a_[i].days->make_call(); call.set_value(value); call.perform(); tessera::refresh();
}
void AlarmClock::set_active(int i,bool on) {
  if (!a_[i].enabled) return;
  if (on) {
    if (!a_[i].enabled->state) {
      a_[i].last_fired_date=0; uint32_t empty=0; a_[i].preference.save(&empty);
    }
    a_[i].enabled->turn_on();
  } else a_[i].enabled->turn_off();
  tessera::refresh();
}
bool AlarmClock::ringing(int i) const { return a_[i].active; }
bool AlarmClock::any_ringing() const { for (auto &s:a_) if(s.active) return true; return false; }
int AlarmClock::first_ringing() const {
  if (audio_owner_>=0 && audio_owner_<COUNT && a_[audio_owner_].active) return audio_owner_;
  for (int i=0;i<COUNT;++i) if(a_[i].active) return i;
  return -1;
}
int AlarmClock::next_alarm() const {
  const uint32_t epoch=tessera::epoch();
  if (!epoch) { for (int i=0;i<COUNT;++i) if(enabled(i)) return i; return -1; }
  const auto now=tessera::local_time(epoch);
  const int now_min=now.hour*60+now.minute;
  int best=-1, best_distance=1000000;
  for (int i=0;i<COUNT;++i) {
    if (!enabled(i)) continue;
    for (int d=0;d<=7;++d) {
      const auto candidate=tessera::local_time(epoch+86400u*static_cast<uint32_t>(d));
      const uint32_t key=candidate.year*10000u+candidate.month*100u+candidate.day;
      if (mode(i)==1 && key != static_cast<uint32_t>(year(i)*10000+month(i)*100+day(i))) continue;
      int w=(candidate.weekday+6)%7;
      if (mode(i)==2 && !weekday_enabled(i,w)) continue;
      int minutes=d*1440 + hour(i)*60 + minute(i) - now_min;
      if (minutes<0) continue;
      if (minutes<best_distance) { best=i; best_distance=minutes; }
      break;
    }
  }
  return best;
}
void AlarmClock::start_alarm_(int i,bool test) {
  Slot &s=a_[i];
  if (s.active) return;
  s.active=true; s.test=test; s.started_ms=millis(); s.snooze_pending=false;
  if (!any_ringing() || audio_owner_<0 || !a_[audio_owner_].active) audio_owner_=i;
  tessera::wake(this);
  if (!tessera::popup_shown(this))
    tessera::show_popup(this,[this](){ return new AlarmPopup(this); },tessera::PopupWidth::WIDE);
  last_song_ms_=0;
  play_current_(millis());
  tessera::refresh();
}
void AlarmClock::start_test() { a_[0].snoozes_used=0; start_alarm_(0,true); }
void AlarmClock::stop_alarm(int i) {
  if (i<0 || i>=COUNT) return;
  Slot &s=a_[i]; s.active=false; s.snooze_pending=false; s.test=false; s.snoozes_used=0;
  refresh_audio_owner_(millis());
  if(!any_ringing() && tessera::popup_shown(this)) tessera::close_popup(this);
  tessera::refresh();
}
void AlarmClock::stop_all() { for(int i=0;i<COUNT;++i) {a_[i].active=false; a_[i].snooze_pending=false; a_[i].snoozes_used=0;} audio_owner_=-1; if(player_) player_->stop(); if(tessera::popup_shown(this)) tessera::close_popup(this); tessera::refresh(); }
void AlarmClock::snooze_alarm(int i) {
  if(i<0 || i>=COUNT || !a_[i].active) return;
  Slot &s=a_[i];
  if(s.test || s.snoozes_used>=3) { stop_alarm(i); return; }
  s.active=false; s.snooze_pending=true; ++s.snoozes_used;
  s.snooze_due_ms=millis()+static_cast<uint32_t>(snooze_minutes(i))*60000u;
  refresh_audio_owner_(millis());
  if (!any_ringing() && tessera::popup_shown(this)) tessera::close_popup(this);
  tessera::refresh();
}
void AlarmClock::refresh_audio_owner_(uint32_t now_ms) {
  int next=first_ringing();
  if (next==audio_owner_ && next>=0 && a_[next].active) return;
  audio_owner_=next;
  if(player_) player_->stop();
  last_song_ms_=0;
  if(next>=0) play_current_(now_ms);
}
void AlarmClock::play_current_(uint32_t now_ms) {
  if(!player_ || audio_owner_<0) return;
  const Slot &s=a_[audio_owner_];
  // Safe cap: 100% in AlarmClock means 15% of ESPHome RTTTL's digital gain.
  const float max_gain = 0.15f * volume(audio_owner_) / 100.0f;
  const float ramp = std::min(1.0f, 0.10f+0.90f * static_cast<float>(now_ms-s.started_ms)/60000.0f);
  player_->set_gain(max_gain*ramp);
  if(!player_->is_playing() && (last_song_ms_==0 || static_cast<uint32_t>(now_ms-last_song_ms_)>=500u)) {
    player_->play(SONGS[tune(audio_owner_)]);
    last_song_ms_=now_ms;
  }
}
bool AlarmClock::matches_(int i,uint32_t candidate_epoch,uint32_t &key) const {
  if(!enabled(i)) return false;
  const auto t=tessera::local_time(candidate_epoch);
  if(t.hour!=hour(i) || t.minute!=minute(i)) return false;
  key=t.year*10000u+t.month*100u+t.day;
  if(mode(i)==1) return t.year==year(i) && t.month==month(i) && t.day==day(i);
  if(mode(i)==2) return weekday_enabled(i,(t.weekday+6)%7);
  return true;
}
void AlarmClock::on_interval(uint32_t now_ms) {
  const uint32_t epoch=tessera::epoch();
  if(epoch) {
    uint32_t minute_index=epoch/60u;
    if(minute_index!=seen_minute_) {
      seen_minute_=minute_index;
      for(int i=0;i<COUNT;++i) {
        if(a_[i].active || a_[i].snooze_pending || !enabled(i)) continue;
        for(uint32_t offset=0;offset<=5;++offset) {
          if (epoch < offset*60u) break;
          uint32_t key=0;
          if(matches_(i,epoch-offset*60u,key) && key != a_[i].last_fired_date) {
            a_[i].last_fired_date=key;
            a_[i].preference.save(&key);
            if(mode(i)!=2) set_active(i,false);
            a_[i].snoozes_used=0;
            start_alarm_(i,false);
            break;
          }
        }
      }
    }
  }
  for(int i=0;i<COUNT;++i) {
    Slot &s=a_[i];
    if(s.snooze_pending && static_cast<int32_t>(now_ms-s.snooze_due_ms)>=0) start_alarm_(i,false);
    if(s.active && static_cast<uint32_t>(now_ms-s.started_ms)>=(s.test?15000u:300000u)) stop_alarm(i);
  }
  if(audio_owner_<0 || !a_[audio_owner_].active) refresh_audio_owner_(now_ms);
  if(audio_owner_>=0) play_current_(now_ms);
}
bool AlarmClock::settings(tessera::SettingsPage &page) {
  page.icon="\U000F0020";
  page.card(text("list_title"),"\U000F0020","overview");
  page.action(text("test"),"\U000F0387",[this](){start_test();});
  page.action(text("stop_all"),"\U000F0020",[this](){stop_all();});
  return true;
}
void AlarmClock::setup() {
  for(int i=0;i<COUNT;++i) {
    a_[i].preference=global_preferences->make_preference<uint32_t>(0xAC4100u+static_cast<uint32_t>(i));
    a_[i].preference.load(&a_[i].last_fired_date);
  }
  add_tile("next_alarm",[this](){return new NextAlarmTile(this);});
  add_card("overview",[this](){return new AlarmOverview(this);},true);
}
} // namespace esphome::alarmclock
