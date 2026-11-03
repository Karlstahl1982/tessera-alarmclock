#include "alarmclock.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>

namespace esphome::alarmclock {
namespace ui = tessera::ui;
using tessera::Font;

namespace {
constexpr const char *DAY_ABBR[] = {"Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"};
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
    tile_ = c.tile;
    w_ = c.width;
    h_ = c.height;
    heading_ = ui::label(c.parent, Font::TITLE, theme::INK);
    clock_ = ui::label(c.parent, Font::VALUE, theme::INK);
    detail_ = ui::label(c.parent, Font::BODY_LARGE, theme::MUTED);
    footer_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    on_tick(0);
  }
  void on_tick(uint32_t) override {
    if (!clock_) return;
    place_label(heading_, 0, ui::px(5), w_, ui::line_height(Font::TITLE), p_->text("hero_title"));
    const int big_h = std::min(h_ / 2, ui::px(85));
    place_label(clock_, 0, std::max(ui::px(25), (h_ - big_h) / 2 - ui::px(10)), w_, big_h, "--:--");
    const int n = p_->next_alarm();
    char value[72];
    if (p_->any_ringing()) {
      ui::set_text(clock_, p_->text("ringing"));
      std::snprintf(value, sizeof(value), "%s", p_->text("ringing_hint"));
    } else if (n < 0) {
      std::snprintf(value, sizeof(value), "%s", p_->text("no_alarms"));
    } else {
      char time[12];
      std::snprintf(time, sizeof(time), "%02d:%02d", p_->hour(n), p_->minute(n));
      ui::set_text(clock_, time);
      std::snprintf(value, sizeof(value), "%s", p_->alarm_name(n));
    }
    const int bottom_y = std::max(ui::px(60), h_ - ui::px(64));
    place_label(detail_, 0, bottom_y, w_, ui::line_height(Font::BODY_LARGE), value);
    char footer[68];
    std::snprintf(footer, sizeof(footer), "%d / 5  |  %s", p_->occupied_count(), p_->text("hero_footer"));
    place_label(footer_, 0, h_ - ui::px(27), w_, ui::px(25), footer);
  }
  void on_theme() override {
    ui::set_color(heading_, theme::INK);
    ui::set_color(clock_, theme::INK);
    ui::set_color(detail_, theme::MUTED);
    ui::set_color(footer_, theme::MUTED);
  }
  void on_tap() override { tessera::open_card(p_->plugin_id(), "overview", "", tile_); }
 private:
  AlarmClock *p_{};
  int tile_{-1}, w_{0}, h_{0};
  lv_obj_t *heading_{}, *clock_{}, *detail_{}, *footer_{};
};

class AlarmOverview final : public tessera::Card {
 public:
  explicit AlarmOverview(AlarmClock *p) : p_(p) {}
  void open(const tessera::CardContext &c) override {
    w_ = c.width;
    h_ = c.height;
    title_ = ui::label(c.parent, Font::TITLE, theme::INK);
    subtitle_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    for (auto &r : rows_) r = ui::label(c.parent, Font::BODY_LARGE, theme::INK);
    for (auto &r : days_) r = ui::label(c.parent, Font::BODY_LARGE, theme::INK);
    back_ = ui::label(c.parent, Font::BODY_LARGE, theme::INK);
    next_ = ui::label(c.parent, Font::BODY_LARGE, theme::INK);
    hour_ = lv_roller_create(c.parent);
    minute_ = lv_roller_create(c.parent);
    day_ = lv_roller_create(c.parent);
    month_ = lv_roller_create(c.parent);
    year_ = lv_roller_create(c.parent);
    const auto hh = list_numbers(0, 23, 2);
    const auto mm = list_numbers(0, 59, 2);
    const auto dd = list_numbers(1, 31, 2);
    const auto mo = list_numbers(1, 12, 2);
    const auto yy = list_numbers(2025, 2045, 4);
    lv_roller_set_options(hour_, hh.c_str(), LV_ROLLER_MODE_INFINITE);
    lv_roller_set_options(minute_, mm.c_str(), LV_ROLLER_MODE_INFINITE);
    lv_roller_set_options(day_, dd.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_options(month_, mo.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_options(year_, yy.c_str(), LV_ROLLER_MODE_NORMAL);
    for (auto *r : {hour_, minute_, day_, month_, year_}) {
      lv_roller_set_visible_row_count(r, 3);
      color_roller(r);
    }
    // Preallocate the 2x3 alarm grid and the four graphical edit cards.
    // All objects are hidden while the wizard's detail screens are visible.
    for (auto &card : list_cards_) make_visual_(c.parent, card);
    for (auto &card : edit_cards_) make_visual_(c.parent, card);
    render_();
  }
  void on_tick(uint32_t) override { if (screen_ == LIST || screen_ == REVIEW) render_(); }
  void on_theme() override { render_(); }
  bool on_back() override {
    if (screen_ != LIST) { screen_ = LIST; render_(); return true; }
    return false;
  }
  void on_tap() override {
    const auto pt = tessera::tap_point();
    if (pt.x < 0 || pt.y < 0) return;
    const int x = pt.x, y = pt.y;
    if (screen_ == LIST) {
      const int gap=ui::px(8), margin=ui::px(8);
      const int top=list_top_();
      const int card_h=list_card_height_();
      const int card_w=(w_-2*margin-gap)/2;
      if (y>=top && card_h>0) {
        const int row=(y-top)/(card_h+gap);
        const int col=(x-margin)/(card_w+gap);
        if (row>=0 && row<3 && col>=0 && col<2) {
          const int card_x=margin+col*(card_w+gap);
          const int card_y=top+row*(card_h+gap);
          if (x>=card_x && x<card_x+card_w && y>=card_y && y<card_y+card_h) {
            const int index=row*2+col;
            if (index<listed_) {
              const int slot=visible_[index];
              // Top-right status pill is an explicit quick toggle.
              if (x>=card_x+card_w-ui::px(52) && y<card_y+ui::px(42))
                p_->set_active(slot,!p_->enabled(slot));
              else begin_edit_(slot);
            } else if (index==listed_ && listed_<AlarmClock::COUNT) begin_new_();
          }
        }
      }
    } else {
      const int btn_y = h_ - ui::px(48);
      if (y >= btn_y) {
        if (x < w_/2) previous_();
        else next_step_();
      } else if (screen_ == EDIT) {
        // Top hero: time opens the wheels, status pill toggles active.
        if (y>=ui::px(53) && y<ui::px(132)) {
          if (x>=w_-ui::px(87)) draft_.active=!draft_.active;
          else { screen_=TIME; init_time_wheels_(); }
        } else if (y>=edit_grid_top_() && y<edit_grid_bottom_()) {
          const int gap=ui::px(8), margin=ui::px(8);
          const int cw=(w_-2*margin-gap)/2;
          const int ch=edit_card_height_();
          const int col=(x-margin)/(cw+gap);
          const int row=(y-edit_grid_top_())/(ch+gap);
          if (col>=0 && col<2 && row>=0 && row<2) {
            const int xx=margin+col*(cw+gap), yy=edit_grid_top_()+row*(ch+gap);
            if (x>=xx && x<xx+cw && y>=yy && y<yy+ch) {
              switch (row*2+col) {
                case 0: screen_=TIME;init_time_wheels_();break;
                case 1: screen_=TYPE;break;
                case 2: screen_=SOUND;break;
                case 3: screen_=OPTIONS;break;
              }
            }
          }
        } else if (y>=h_-ui::px(96) && y<btn_y) screen_=CONFIRM_DELETE;
      } else if (screen_ == TYPE) {
        const int top=detail_top_(), gap=ui::px(9), row_h=type_card_height_();
        if (y>=top) {
          const int row=(y-top)/(row_h+gap);
          const int yy=top+row*(row_h+gap);
          if (row>=0 && row<3 && y>=yy && y<yy+row_h) draft_.mode=row;
        }
      } else if (screen_ == DAYS) {
        const int top=ui::px(104), gap=ui::px(12);
        const int cw=(w_-ui::px(28)-3*gap)/4;
        const int col=(x-ui::px(14))/(cw+gap);
        const int row=(y-top)/(ui::px(64)+gap);
        if (col>=0 && col<4 && row>=0 && row<2) {
          int xx=ui::px(14)+col*(cw+gap), yy=top+row*(ui::px(64)+gap);
          const int day=row*4+col;
          if(day<7 && x>=xx && x<xx+cw && y>=yy && y<yy+ui::px(64)) draft_.days ^= 1u<<day;
        }
      } else if (screen_ == SOUND) {
        const int top=detail_top_(), gap=ui::px(8), card_h=detail_grid_height_();
        const int card_w=(w_-ui::px(16)-gap)/2;
        const int col=(x-ui::px(8))/(card_w+gap);
        const int row=(y-top)/(card_h+gap);
        const int xx=ui::px(8)+col*(card_w+gap), yy=top+row*(card_h+gap);
        const int index=row*2+col;
        if(col>=0 && col<2 && row>=0 && row<3 && x>=xx && x<xx+card_w && y>=yy && y<yy+card_h) {
          if(index<5) draft_.tone=index;
          else p_->preview_tone(draft_.tone);
        }
      } else if (screen_ == OPTIONS) {
        const int top=detail_top_(), gap=ui::px(8), ch=options_card_height_();
        const int cw=(w_-ui::px(16)-gap)/2;
        const int col=(x-ui::px(8))/(cw+gap), row=(y-top)/(ch+gap);
        const int xx=ui::px(8)+col*(cw+gap), yy=top+row*(ch+gap);
        if(col>=0 && col<2 && row>=0 && row<2 && x>=xx && x<xx+cw && y>=yy && y<yy+ch) {
          switch(row*2+col) {
            case 0: cycle_name_(); break;
            case 1: draft_.volume=clamp_int(draft_.volume+(x<xx+cw/2?-10:10),10,100); break;
            case 2: draft_.snooze=draft_.snooze==5?10:5; break;
            case 3: draft_.active=!draft_.active; break;
          }
        }
      } else if (screen_==REVIEW && !new_ && y>=h_-ui::px(101) && y<h_-ui::px(52)) {
        screen_=CONFIRM_DELETE;
      }
    }
    render_();
  }
 private:
  enum Screen { LIST, EDIT, TYPE, TIME, DATE, DAYS, SOUND, OPTIONS, REVIEW, CONFIRM_DELETE };
  struct Draft {
    int hour{7}, minute{0}, year{2026}, month{11}, day{4};
    int mode{0}, tone{1}, volume{60}, snooze{5};
    uint8_t days{31};
    bool active{true};
    std::string name{"Wecker"};
  } draft_{};
  int list_top_() const { return ui::px(53); }
  int list_rh_() const { return std::max(1, (h_-list_top_()-ui::px(58))/6); }
  int option_rh_() const { return std::max(ui::px(35),(h_-ui::px(140))/5); }
  void begin_edit_(int slot) {
    selected_=slot; new_=false;
    draft_.hour=p_->hour(slot); draft_.minute=p_->minute(slot);
    draft_.year=p_->year(slot); draft_.month=p_->month(slot); draft_.day=p_->day(slot);
    draft_.mode=p_->mode(slot); draft_.tone=p_->tune(slot);
    draft_.volume=p_->volume(slot); draft_.snooze=p_->snooze_minutes(slot);
    draft_.days=p_->weekdays(slot); draft_.active=p_->enabled(slot);
    draft_.name=p_->alarm_name(slot);
    screen_=EDIT;
  }
  void begin_new_() {
    int free_slot=-1;
    for (int i=0;i<AlarmClock::COUNT;++i) if(!p_->occupied(i)) {free_slot=i; break;}
    if(free_slot<0) return;
    selected_=free_slot; new_=true; draft_=Draft{};
    char b[24]; std::snprintf(b,sizeof(b),"Wecker %d",selected_+1); draft_.name=b;
    if(p_->time_valid()) {
      auto dt=tessera::local_time(tessera::epoch());
      draft_.year=dt.year; draft_.month=dt.month; draft_.day=dt.day;
    }
    screen_=TYPE;
  }
  void init_time_wheels_() {
    lv_roller_set_selected(hour_, draft_.hour, LV_ANIM_OFF);
    lv_roller_set_selected(minute_, draft_.minute, LV_ANIM_OFF);
  }
  void init_date_wheels_() {
    lv_roller_set_selected(day_, clamp_int(draft_.day,1,31)-1,LV_ANIM_OFF);
    lv_roller_set_selected(month_,clamp_int(draft_.month,1,12)-1,LV_ANIM_OFF);
    lv_roller_set_selected(year_,clamp_int(draft_.year,2025,2045)-2025,LV_ANIM_OFF);
  }
  void capture_time_() {
    draft_.hour=lv_roller_get_selected(hour_);
    draft_.minute=lv_roller_get_selected(minute_);
  }
  void capture_date_() {
    draft_.year=2025+lv_roller_get_selected(year_);
    draft_.month=1+lv_roller_get_selected(month_);
    draft_.day=std::min(1+int(lv_roller_get_selected(day_)),days_in_month(draft_.year,draft_.month));
  }
  void cycle_name_() {
    static constexpr const char *PRESETS[]={"Wecker","Schule","Arbeit","Termin","Früh","Freizeit"};
    int current=-1;
    for (int i=0;i<6;++i) if(draft_.name==PRESETS[i]) current=i;
    draft_.name=PRESETS[(current+1)%6];
  }
  void previous_() {
    // Edits return to the graphic editor instead of restarting the new-alarm wizard.
    if (!new_) {
      if (screen_==EDIT) { screen_=LIST; return; }
      if (screen_==TIME) capture_time_();
      if (screen_==DATE) capture_date_();
      screen_=EDIT;
      return;
    }
    switch(screen_) {
      case TYPE: screen_=LIST; break;
      case TIME: capture_time_(); screen_=TYPE; break;
      case DATE: capture_date_(); screen_=TIME; init_time_wheels_(); break;
      case DAYS: screen_=TIME; init_time_wheels_(); break;
      case SOUND: {
        if(draft_.mode==1) { screen_=DATE; init_date_wheels_(); }
        else if(draft_.mode==2) { screen_=DAYS; }
        else { screen_=TIME; init_time_wheels_(); }
        break;
      }
      case OPTIONS: screen_=SOUND; break;
      case REVIEW: screen_=OPTIONS; break;
      case CONFIRM_DELETE: screen_=REVIEW; break;
      default: screen_=LIST; break;
    }
  }
  void next_step_() {
    if (!new_) {
      switch(screen_) {
        case EDIT: save_(); screen_=LIST; return;
        case TIME: capture_time_(); screen_=EDIT; return;
        case TYPE:
          if (draft_.mode==1) { screen_=DATE; init_date_wheels_(); }
          else if (draft_.mode==2) screen_=DAYS;
          else screen_=EDIT;
          return;
        case DATE: capture_date_(); screen_=EDIT; return;
        case DAYS: if (!draft_.days) draft_.days=31; screen_=EDIT; return;
        case SOUND: case OPTIONS: screen_=EDIT; return;
        case CONFIRM_DELETE:
          p_->set_active(selected_,false);
          p_->set_occupied(selected_,false);
          screen_=LIST;
          return;
        default: screen_=EDIT; return;
      }
    }
    switch(screen_) {
      case TYPE: screen_=TIME; init_time_wheels_(); break;
      case TIME: {
        capture_time_();
        if(draft_.mode==1){screen_=DATE;init_date_wheels_();}
        else if(draft_.mode==2){screen_=DAYS;}
        else {screen_=SOUND;}
        break;
      }
      case DATE: capture_date_(); screen_=SOUND; break;
      case DAYS: if(!draft_.days) draft_.days=31; screen_=SOUND; break;
      case SOUND: screen_=OPTIONS; break;
      case OPTIONS: screen_=REVIEW; break;
      case REVIEW: save_(); screen_=LIST; break;
      case CONFIRM_DELETE: p_->set_active(selected_,false); p_->set_occupied(selected_,false);
                           screen_=LIST; break;
      default: screen_=LIST; break;
    }
  }
  void save_() {
    if (selected_<0) return;
    p_->set_active(selected_,false);
    p_->set_alarm_time(selected_,draft_.hour,draft_.minute);
    p_->set_alarm_date(selected_,draft_.year,draft_.month,draft_.day);
    p_->set_alarm_mode(selected_,draft_.mode);
    p_->set_alarm_tone(selected_,draft_.tone);
    p_->set_alarm_volume(selected_,draft_.volume);
    p_->set_snooze_minutes(selected_,draft_.snooze);
    for(int d=0;d<7;++d) p_->set_weekday(selected_,d,(draft_.days&(1<<d))!=0);
    p_->set_alarm_name(selected_,draft_.name);
    p_->set_occupied(selected_,true);
    p_->set_active(selected_,draft_.active);
  }
  // These are simple LVGL primitives rather than emoji font glyphs: they
  // render on the Guition even if the firmware contains only limited fonts.
  struct VisualCard {
    lv_obj_t *surface{}, *disc{}, *hand_a{}, *hand_b{};
    lv_obj_t *title{}, *value{}, *meta{}, *status{};
  };
  void make_visual_(lv_obj_t *parent, VisualCard &c) {
    c.surface=lv_obj_create(parent);
    c.disc=lv_obj_create(parent);
    c.hand_a=lv_obj_create(parent);
    c.hand_b=lv_obj_create(parent);
    for (auto *obj:{c.surface,c.disc,c.hand_a,c.hand_b}) {
      lv_obj_remove_flag(obj,LV_OBJ_FLAG_CLICKABLE);
      lv_obj_remove_flag(obj,LV_OBJ_FLAG_SCROLLABLE);
      lv_obj_set_style_border_width(obj,0,0);
      lv_obj_set_style_pad_all(obj,0,0);
    }
    c.title=ui::label(parent,Font::BODY,theme::INK);
    c.value=ui::label(parent,Font::VALUE,theme::INK);
    c.meta=ui::label(parent,Font::BODY,theme::MUTED);
    c.status=ui::label(parent,Font::BODY,theme::INK);
  }
  void show_visual_(VisualCard &c,bool visible) {
    for (auto *obj:{c.surface,c.disc,c.hand_a,c.hand_b,c.title,c.value,c.meta,c.status})
      toggle_hidden(obj,visible);
  }
  void paint_visual_(VisualCard &c,int x,int y,int w,int h,
                     const char *title,const char *value,const char *meta,
                     const char *status,bool active,bool add=false,bool editor=false,int icon_mode=0) {
    // Layout zones are disjoint: header 0..36, clock 38..(h-28), footer (h-25)..h.
    // This fixes the v0.3.0 title/time overlap on the 480x480 Guition.
    lv_obj_set_pos(c.surface,x,y);
    lv_obj_set_size(c.surface,w,h);
    lv_obj_set_style_radius(c.surface,ui::px(14),0);
    lv_obj_set_style_bg_color(c.surface,ui::color(theme::CARD),0);
    lv_obj_set_style_bg_opa(c.surface,LV_OPA_COVER,0);
    lv_obj_set_style_border_width(c.surface,active?ui::px(2):ui::px(1),0);
    lv_obj_set_style_border_color(c.surface,ui::color(active?theme::ACCENT:theme::MUTED),0);
    const int d=ui::px(24), circle_x=x+ui::px(10), circle_y=y+ui::px(8);
    lv_obj_set_pos(c.disc,circle_x,circle_y);
    lv_obj_set_size(c.disc,d,d);
    lv_obj_set_style_radius(c.disc,d/2,0);
    lv_obj_set_style_bg_opa(c.disc,LV_OPA_COVER,0);
    lv_obj_set_style_bg_color(c.disc,ui::color(active?theme::ACCENT:theme::MUTED),0);
    const int mx=circle_x+d/2, my=circle_y+d/2;
    if(icon_mode==1) { // repeating: two horizontal strokes
      lv_obj_set_pos(c.hand_a,circle_x+ui::px(5),circle_y+ui::px(7));
      lv_obj_set_size(c.hand_a,ui::px(14),ui::px(2));
      lv_obj_set_pos(c.hand_b,circle_x+ui::px(5),circle_y+ui::px(15));
      lv_obj_set_size(c.hand_b,ui::px(14),ui::px(2));
    } else if(icon_mode==2) { // sound: equalizer
      lv_obj_set_pos(c.hand_a,circle_x+ui::px(7),circle_y+ui::px(5));
      lv_obj_set_size(c.hand_a,ui::px(3),ui::px(15));
      lv_obj_set_pos(c.hand_b,circle_x+ui::px(14),circle_y+ui::px(9));
      lv_obj_set_size(c.hand_b,ui::px(3),ui::px(9));
    } else if(icon_mode==3) { // settings: slider
      lv_obj_set_pos(c.hand_a,circle_x+ui::px(5),my-ui::px(1));
      lv_obj_set_size(c.hand_a,ui::px(14),ui::px(2));
      lv_obj_set_pos(c.hand_b,mx-ui::px(2),circle_y+ui::px(6));
      lv_obj_set_size(c.hand_b,ui::px(4),ui::px(12));
    } else { // clock: hands
      lv_obj_set_pos(c.hand_a,mx-ui::px(1),circle_y+ui::px(5));
      lv_obj_set_size(c.hand_a,ui::px(2),ui::px(8));
      lv_obj_set_pos(c.hand_b,mx,my-ui::px(1));
      lv_obj_set_size(c.hand_b,ui::px(6),ui::px(2));
    }
    for (auto *line:{c.hand_a,c.hand_b}) {
      lv_obj_set_style_bg_opa(line,LV_OPA_COVER,0);
      lv_obj_set_style_bg_color(line,ui::color(theme::ON_ACCENT),0);
      lv_obj_set_style_radius(line,ui::px(1),0);
    }
    if(add) {
      // One strong central plus with a short caption below. No overlapping icon.
      toggle_hidden(c.disc,false);toggle_hidden(c.hand_a,false);toggle_hidden(c.hand_b,false);
      place_label(c.value,x+ui::px(6),y+h/2-ui::px(31),w-ui::px(12),ui::px(58),"+");
      lv_obj_set_style_text_font(c.value,ui::font(Font::VALUE),0);
      place_label(c.title,x+ui::px(5),y+h-ui::px(32),w-ui::px(10),ui::px(24),title);
      lv_obj_set_style_text_font(c.title,ui::font(Font::BODY),0);
      toggle_hidden(c.meta,false);toggle_hidden(c.status,false);
    } else if(editor) {
      // Graphical settings card: short heading by icon and value on its own line.
      const bool compact_editor=h<ui::px(90);
      place_label(c.title,x+ui::px(39),y+(compact_editor?ui::px(2):ui::px(8)),
                  w-ui::px(45),compact_editor?ui::px(24):ui::px(29),title);
      lv_obj_set_style_text_font(c.title,ui::font(Font::BODY),0);
      place_label(c.value,x+ui::px(8),y+(compact_editor?ui::px(26):std::max(ui::px(42),h-ui::px(48))),
                  w-ui::px(16),compact_editor?ui::px(26):ui::px(39),value);
      lv_obj_set_style_text_font(c.value,ui::font(compact_editor?Font::BODY:Font::BODY_LARGE),0);
      toggle_hidden(c.meta,false);toggle_hidden(c.status,false);
    } else {
      const int status_width=(status && *status)?ui::px(43):0;
      // Header: name always above the time; status gets its own top-right pill.
      const int name_x=x+ui::px(39);
      const int name_w=std::max(ui::px(45),w-ui::px(46)-status_width);
      const bool compact_clock=h<ui::px(112);
      place_label(c.title,name_x,y+(compact_clock?ui::px(4):ui::px(7)),name_w,
                  compact_clock?ui::px(24):ui::px(27),title);
      lv_obj_set_style_text_font(c.title,ui::font(Font::BODY),0);
      // Center clock has a *separate* band; no label intersects its bounding box.
      const int clock_top=compact_clock?ui::px(31):ui::px(36);
      const int clock_h=compact_clock?std::max(ui::px(20),h-ui::px(61)):h-ui::px(72);
      place_label(c.value,x+ui::px(6),y+clock_top,w-ui::px(12),clock_h,value);
      lv_obj_set_style_text_font(c.value,ui::font(compact_clock?Font::BODY_LARGE:Font::VALUE),0);
      if(h>=ui::px(91)) {
        place_label(c.meta,x+ui::px(8),y+h-ui::px(26),w-ui::px(16),ui::px(21),meta);
        lv_obj_set_style_text_font(c.meta,ui::font(Font::BODY),0);
      } else toggle_hidden(c.meta,false);
      if(status_width) {
        place_label(c.status,x+w-status_width-ui::px(7),y+ui::px(8),status_width,ui::px(25),status);
        lv_obj_set_style_text_font(c.status,ui::font(Font::BODY),0);
        lv_obj_set_style_bg_opa(c.status,LV_OPA_COVER,0);
        lv_obj_set_style_bg_color(c.status,ui::color(active?theme::ACCENT:theme::CARD),0);
        lv_obj_set_style_text_color(c.status,ui::color(active?theme::ON_ACCENT:theme::MUTED),0);
        lv_obj_set_style_radius(c.status,ui::px(9),0);
      } else toggle_hidden(c.status,false);
    }
  }
  int list_card_height_() const {
    return std::max(1,(h_-list_top_()-ui::px(16)-2*ui::px(8))/3);
  }
  int detail_top_() const { return ui::px(70); }
  int type_card_height_() const { return std::max(1,(h_-detail_top_()-ui::px(62)-2*ui::px(9))/3); }
  int detail_grid_height_() const { return std::max(1,(h_-detail_top_()-ui::px(61)-2*ui::px(8))/3); }
  int options_card_height_() const { return std::max(1,(h_-detail_top_()-ui::px(63)-ui::px(8))/2); }
  int edit_grid_top_() const { return ui::px(139); }
  int edit_grid_bottom_() const { return h_-ui::px(103); }
  int edit_card_height_() const {
    return std::max(1,(edit_grid_bottom_()-edit_grid_top_()-ui::px(8))/2);
  }
  void hide_all_() {
    for(auto *r:rows_) toggle_hidden(r,false);
    for(auto *d:days_) toggle_hidden(d,false);
    for(auto *r:{hour_,minute_,day_,month_,year_}) toggle_hidden(r,false);
    for(auto &c:list_cards_) show_visual_(c,false);
    for(auto &c:edit_cards_) show_visual_(c,false);
    toggle_hidden(title_,true); toggle_hidden(subtitle_,true);
    toggle_hidden(back_,false);toggle_hidden(next_,false);
  }
  void chip_(lv_obj_t *label,int x,int y,int width,int height,const char *value,bool selected=false) {
    place_label(label,x,y,width,height,value);
    lv_obj_set_style_bg_opa(label,LV_OPA_COVER,0);
    lv_obj_set_style_bg_color(label,ui::color(selected?theme::ACCENT:theme::CARD),0);
    lv_obj_set_style_radius(label,ui::px(10),0);
    lv_obj_set_style_pad_hor(label,ui::px(4),0);
    lv_obj_set_style_text_color(label,ui::color(selected?theme::ON_ACCENT:theme::INK),0);
  }
  void footer_(const char *next) {
    chip_(back_,ui::px(4),h_-ui::px(47),w_/2-ui::px(7),ui::px(43),p_->text("back"));
    chip_(next_,w_/2+ui::px(3),h_-ui::px(47),w_/2-ui::px(7),ui::px(43),next,true);
    toggle_hidden(back_,true);toggle_hidden(next_,true);
  }
  void heading_(const char *title,const char *sub) {
    place_label(title_,0,0,w_,ui::px(31),title);
    place_label(subtitle_,0,ui::px(31),w_,ui::px(25),sub);
  }
  void render_() {
    if (!title_) return;
    hide_all_();
    char b[140];
    if (screen_==LIST) {
      heading_(p_->text("list_title"),"");
      listed_=0;
      for(int i=0;i<AlarmClock::COUNT;++i)
        if(p_->occupied(i)) visible_[listed_++]=i;
      char count[16];
      std::snprintf(count,sizeof(count),"%d / 5",listed_);
      chip_(rows_[8],w_-ui::px(76),ui::px(3),ui::px(66),ui::px(28),count);
      toggle_hidden(rows_[8],true);
      const int gap=ui::px(8), margin=ui::px(8);
      const int cw=(w_-2*margin-gap)/2;
      const int ch=list_card_height_();
      for(int n=0;n<6;++n) {
        if(n>=listed_ && (n!=listed_ || listed_==AlarmClock::COUNT)) continue;
        const int x=margin+(n%2)*(cw+gap);
        const int y=list_top_()+(n/2)*(ch+gap);
        auto &card=list_cards_[n];
        show_visual_(card,true);
        if(n==listed_) {
          paint_visual_(card,x,y,cw,ch,p_->text("grid_add"),"","","",true,true,false);
        } else {
          const int slot=visible_[n];
          const bool active=p_->enabled(slot);
          char t[10];
          std::snprintf(t,sizeof(t),"%02d:%02d",p_->hour(slot),p_->minute(slot));
          const char *freq=p_->mode(slot)==2?p_->text("grid_weekly"):
                            p_->mode(slot)==1?p_->text("grid_once_date"):p_->text("grid_once");
          paint_visual_(card,x,y,cw,ch,p_->alarm_name(slot),t,freq,
                        active?p_->text("grid_on"):p_->text("grid_off"),active);
        }
      }
      return;
    }
    const char *step="";
    switch(screen_) {
      case EDIT: step=p_->text("edit_alarm");break;
      case TYPE: step=p_->text("step_type");break;
      case TIME: step=p_->text("step_time");break;
      case DATE: step=p_->text("step_date");break;
      case DAYS: step=p_->text("step_days");break;
      case SOUND: step=p_->text("step_sound");break;
      case OPTIONS: step=p_->text("step_options");break;
      case REVIEW: step=p_->text("step_review");break;
      case CONFIRM_DELETE: step=p_->text("delete_alarm");break;
      default:break;
    }
    heading_(step,draft_.name.c_str());
    footer_((screen_==REVIEW || screen_==EDIT)?p_->text("save"):(screen_==CONFIRM_DELETE)?p_->text("delete_alarm"):p_->text("continue"));
    if (screen_==EDIT) {
      heading_(draft_.name.c_str(),p_->text("edit_short"));
      // Prominent clock hero; tap time to open wheels, tap status to toggle.
      std::snprintf(b,sizeof(b),"%02d:%02d",draft_.hour,draft_.minute);
      place_label(rows_[7],ui::px(12),ui::px(61),w_-ui::px(111),ui::px(61),b);
      lv_obj_set_style_text_font(rows_[7],ui::font(Font::VALUE),0);
      toggle_hidden(rows_[7],true);
      chip_(rows_[8],w_-ui::px(88),ui::px(70),ui::px(79),ui::px(39),
            draft_.active?p_->text("grid_on"):p_->text("grid_off"),draft_.active);
      toggle_hidden(rows_[8],true);
      // Two-by-two feature tiles keep the settings scannable at 4 inches.
      const char *captions[4]={p_->text("grid_time"),p_->text("grid_repeat"),
                                p_->text("grid_tone"),p_->text("grid_audio")};
      char values[4][58];
      std::snprintf(values[0],sizeof(values[0]),"%02d:%02d",draft_.hour,draft_.minute);
      const char *frequency=draft_.mode==2?p_->text("grid_weekly"):
                            draft_.mode==1?p_->text("grid_once_date"):p_->text("grid_once");
      std::snprintf(values[1],sizeof(values[1]),"%s",frequency);
      const char *tone=p_->text(draft_.tone==0?"tone_classic":draft_.tone==1?"tone_gentle":
                                draft_.tone==2?"tone_digital":draft_.tone==3?"tone_sunrise":"tone_urgent");
      std::snprintf(values[2],sizeof(values[2]),"%s",tone);
      std::snprintf(values[3],sizeof(values[3]),"%d%% · %d Min",draft_.volume,draft_.snooze);
      const int gap=ui::px(8), margin=ui::px(8);
      const int cw=(w_-2*margin-gap)/2;
      const int ch=edit_card_height_();
      for(int n=0;n<4;++n) {
        const int x=margin+(n%2)*(cw+gap),y=edit_grid_top_()+(n/2)*(ch+gap);
        show_visual_(edit_cards_[n],true);
        paint_visual_(edit_cards_[n],x,y,cw,ch,captions[n],values[n],"","",true,false,true,n);
      }
      chip_(rows_[6],ui::px(27),h_-ui::px(96),w_-ui::px(54),ui::px(40),p_->text("delete_alarm"));
      toggle_hidden(rows_[6],true);
    } else if(screen_==TYPE) {
      const int top=detail_top_(),gap=ui::px(9),rh=type_card_height_();
      for(int i=0;i<3;++i) {
        auto &c=list_cards_[i]; show_visual_(c,true);
        const char *name=i==0?p_->text("grid_once"):i==1?p_->text("grid_once_date"):p_->text("grid_weekly");
        const char *info=i==0?p_->text("mode_next"):i==1?p_->text("mode_date"):p_->text("step_days");
        paint_visual_(c,ui::px(12),top+i*(rh+gap),w_-ui::px(24),rh,name,info,"","",i==draft_.mode,false,true,i==2?1:0);
      }
    } else if (screen_==TIME || screen_==DATE) {
      const bool is_time=screen_==TIME;
      const int count=is_time?2:3;
      const int rw=(w_-ui::px(20))/count;
      const int rh=std::min(ui::px(168),std::max(ui::px(78),h_-ui::px(179)));
      const int ry=std::max(ui::px(104), (h_-rh)/2+ui::px(5));
      if(is_time) std::snprintf(b,sizeof(b),"%02d : %02d",draft_.hour,draft_.minute);
      else std::snprintf(b,sizeof(b),"%02d.%02d.%04d",draft_.day,draft_.month,draft_.year);
      place_label(rows_[0],ui::px(8),ui::px(59),w_-ui::px(16),ui::px(42),b);
      lv_obj_set_style_text_font(rows_[0],ui::font(Font::BODY_LARGE),0);
      toggle_hidden(rows_[0],true);
      for(int j=0;j<count;++j) {
        lv_obj_t *r=is_time?(j==0?hour_:minute_):(j==0?day_:j==1?month_:year_);
        lv_obj_set_pos(r,ui::px(8)+j*rw,ry);
        lv_obj_set_size(r,rw-ui::px(4),rh);
        color_roller(r);
        toggle_hidden(r,true);
      }
    } else if(screen_==DAYS) {
      // Two rows of tactile weekday chips, selected days highlighted.
      const int top=ui::px(104),gap=ui::px(12);
      const int cw=(w_-ui::px(28)-3*gap)/4;
      for(int i=0;i<7;++i) {
        const int col=i%4,row=i/4;
        chip_(days_[i],ui::px(14)+col*(cw+gap),top+row*(ui::px(64)+gap),cw,ui::px(64),DAY_ABBR[i],(draft_.days&(1<<i))!=0);
        lv_obj_set_style_radius(days_[i],ui::px(23),0);
        toggle_hidden(days_[i],true);
      }
      place_label(rows_[0],ui::px(12),ui::px(270),w_-ui::px(24),ui::px(36),p_->text("days_select"));
      toggle_hidden(rows_[0],true);
    } else if(screen_==SOUND) {
      const int top=detail_top_(),gap=ui::px(8);
      const int cw=(w_-ui::px(16)-gap)/2,ch=detail_grid_height_();
      for(int n=0;n<6;++n) {
        auto &c=list_cards_[n]; show_visual_(c,true);
        const bool preview=n==5;
        const char *tone=preview?p_->text("preview_short"):
            p_->text(n==0?"tone_classic":n==1?"tone_gentle":n==2?"tone_digital":n==3?"tone_sunrise":"tone_urgent");
        const char *subtitle=preview?p_->text("preview_detail"):
            p_->text(n==0?"tone_classic_hint":n==1?"tone_gentle_hint":n==2?"tone_digital_hint":n==3?"tone_sunrise_hint":"tone_urgent_hint");
        paint_visual_(c,ui::px(8)+(n%2)*(cw+gap),top+(n/2)*(ch+gap),cw,ch,tone,subtitle,"","",
                      preview || draft_.tone==n,false,true,2);
      }
    } else if(screen_==OPTIONS) {
      const int top=detail_top_(),gap=ui::px(8);
      const int cw=(w_-ui::px(16)-gap)/2,ch=options_card_height_();
      char values[4][52];
      std::snprintf(values[0],sizeof(values[0]),"%s",draft_.name.c_str());
      std::snprintf(values[1],sizeof(values[1]),"-  %d%%  +",draft_.volume);
      std::snprintf(values[2],sizeof(values[2]),"%d min",draft_.snooze);
      std::snprintf(values[3],sizeof(values[3]),"%s",draft_.active?p_->text("grid_on"):p_->text("grid_off"));
      const char *titles[4]={p_->text("grid_name"),p_->text("volume"),p_->text("snooze"),p_->text("enabled")};
      for(int n=0;n<4;++n) {
        auto &c=list_cards_[n];show_visual_(c,true);
        paint_visual_(c,ui::px(8)+(n%2)*(cw+gap),top+(n/2)*(ch+gap),cw,ch,titles[n],values[n],"","",
                      n==3?draft_.active:true,false,true,n==1?3:0);
      }
    } else if(screen_==REVIEW) {
      std::snprintf(b,sizeof(b),"%02d:%02d",draft_.hour,draft_.minute);
      place_label(rows_[7],ui::px(10),ui::px(60),w_-ui::px(20),ui::px(70),b);
      lv_obj_set_style_text_font(rows_[7],ui::font(Font::VALUE),0);
      toggle_hidden(rows_[7],true);
      const char *titles[4]={p_->text("grid_repeat"),p_->text("grid_tone"),p_->text("volume"),p_->text("snooze")};
      char vals[4][56];
      const char *mode=draft_.mode==0?p_->text("grid_once"):draft_.mode==1?p_->text("grid_once_date"):p_->text("grid_weekly");
      const char *tone=p_->text(draft_.tone==0?"tone_classic":draft_.tone==1?"tone_gentle":
                                draft_.tone==2?"tone_digital":draft_.tone==3?"tone_sunrise":"tone_urgent");
      std::snprintf(vals[0],sizeof(vals[0]),"%s",mode);
      std::snprintf(vals[1],sizeof(vals[1]),"%s",tone);
      std::snprintf(vals[2],sizeof(vals[2]),"%d%%",draft_.volume);
      std::snprintf(vals[3],sizeof(vals[3]),"%d min",draft_.snooze);
      const int gap=ui::px(8), margin=ui::px(8);
      const int cw=(w_-2*margin-gap)/2,ch=edit_card_height_();
      for(int n=0;n<4;++n) {
        const int x=margin+(n%2)*(cw+gap),y=edit_grid_top_()+(n/2)*(ch+gap);
        auto &c=edit_cards_[n];show_visual_(c,true);
        paint_visual_(c,x,y,cw,ch,titles[n],vals[n],"","",true,false,true,n==0?1:n==1?2:3);
      }
      if(!new_) {chip_(rows_[6],ui::px(20),h_-ui::px(99),w_-ui::px(40),ui::px(39),p_->text("delete_alarm"));toggle_hidden(rows_[6],true);}
    } else if(screen_==CONFIRM_DELETE) {
      const int card_h=std::min(ui::px(150),h_-ui::px(130));
      const int card_y=(h_-card_h)/2-ui::px(10);
      auto &c=list_cards_[0];show_visual_(c,true);
      paint_visual_(c,ui::px(14),card_y,w_-ui::px(28),card_h,
                    p_->text("delete_alarm"),p_->text("delete_confirm"),"","",false,false,true);
    }
  }
  AlarmClock *p_{};
  Screen screen_{LIST};
  bool new_{false};
  int selected_{-1}, w_{0}, h_{0}, listed_{0};
  int visible_[AlarmClock::COUNT]{};
  lv_obj_t *title_{},*subtitle_{},*rows_[9]{},*days_[7]{},*back_{},*next_{};
  lv_obj_t *hour_{},*minute_{},*day_{},*month_{},*year_{};
  VisualCard list_cards_[6]{}, edit_cards_[4]{};
};



// System-layer alarm. Unlike a Tessera popup it cannot be dismissed by a tap
// outside the card, a swipe, or a normal page change. The full-screen opaque
// LVGL object intercepts all touch input until Stop or Snooze is pressed.
void AlarmClock::on_overlay_stop_(lv_event_t *event) {
  auto *self=static_cast<AlarmClock *>(lv_event_get_user_data(event));
  if (self) self->stop_alarm(self->first_ringing());
}
void AlarmClock::on_overlay_snooze_(lv_event_t *event) {
  auto *self=static_cast<AlarmClock *>(lv_event_get_user_data(event));
  if (self) {
    int i=self->first_ringing();
    if (i>=0 && self->a_[i].snoozes_used<3 && !self->a_[i].test) self->snooze_alarm(i);
  }
}
void AlarmClock::update_alarm_overlay_() {
  const int active=first_ringing();
  if (active<0) {
    if (alarm_overlay_) {
      lv_obj_delete(alarm_overlay_);
      alarm_overlay_=nullptr;
      overlay_name_=overlay_clock_=overlay_state_=overlay_stop_=overlay_snooze_=nullptr;
    }
    return;
  }
  if (!alarm_overlay_) {
    lv_display_t *display=lv_display_get_default();
    if (!display) return;
    overlay_width_=lv_display_get_horizontal_resolution(display);
    overlay_height_=lv_display_get_vertical_resolution(display);
    // The system layer is above regular cards and Tessera's popup layer.
    alarm_overlay_=lv_obj_create(lv_layer_sys());
    lv_obj_set_pos(alarm_overlay_,0,0);
    lv_obj_set_size(alarm_overlay_,overlay_width_,overlay_height_);
    lv_obj_set_style_radius(alarm_overlay_,0,0);
    lv_obj_set_style_border_width(alarm_overlay_,0,0);
    lv_obj_set_style_bg_color(alarm_overlay_,lv_color_hex(0x101827),0);
    lv_obj_set_style_bg_opa(alarm_overlay_,LV_OPA_COVER,0);
    lv_obj_add_flag(alarm_overlay_,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(alarm_overlay_,LV_OBJ_FLAG_SCROLLABLE);
    overlay_name_=ui::label(alarm_overlay_,Font::TITLE,theme::INK);
    overlay_clock_=ui::label(alarm_overlay_,Font::VALUE,theme::INK);
    overlay_state_=ui::label(alarm_overlay_,Font::BODY_LARGE,theme::INK);
    const int margin=std::max(12,overlay_width_/22);
    const int gap=std::max(10,overlay_width_/35);
    const int button_width=(overlay_width_-2*margin-gap)/2;
    const int button_y=overlay_height_*74/100;
    const int button_height=std::min(78,overlay_height_*18/100);
    auto add_button=[&](int x,lv_color_t color,lv_event_cb_t cb) -> lv_obj_t * {
      // A clickable base object works even when LVGL's optional Button widget
      // is disabled in the firmware configuration (LV_USE_BUTTON=0).
      lv_obj_t *btn=lv_obj_create(alarm_overlay_);
      lv_obj_add_flag(btn,LV_OBJ_FLAG_CLICKABLE);
      lv_obj_remove_flag(btn,LV_OBJ_FLAG_SCROLLABLE);
      lv_obj_set_style_bg_opa(btn,LV_OPA_COVER,0);
      lv_obj_set_style_border_width(btn,0,0);
      lv_obj_set_pos(btn,x,button_y);
      lv_obj_set_size(btn,button_width,button_height);
      lv_obj_set_style_radius(btn,14,0);
      lv_obj_set_style_bg_color(btn,color,0);
      lv_obj_add_event_cb(btn,cb,LV_EVENT_CLICKED,this);
      return btn;
    };
    lv_obj_t *snooze_btn=add_button(margin,lv_color_hex(0x245C8D),on_overlay_snooze_);
    lv_obj_t *stop_btn=add_button(margin+button_width+gap,lv_color_hex(0xD14D56),on_overlay_stop_);
    overlay_snooze_=ui::label(snooze_btn,Font::BODY_LARGE,theme::INK);
    overlay_stop_=ui::label(stop_btn,Font::BODY_LARGE,theme::INK);
    for (auto *o:{overlay_name_,overlay_clock_,overlay_state_,overlay_snooze_,overlay_stop_})
      lv_obj_set_style_text_color(o,lv_color_hex(0xFFFFFF),0);
    place_label(overlay_snooze_,0,0,button_width,button_height,text("snooze"));
    place_label(overlay_stop_,0,0,button_width,button_height,text("stop"));
  }
  char value[96];
  std::snprintf(value,sizeof(value),"%s",alarm_name(active));
  place_label(overlay_name_,12,overlay_height_*16/100,overlay_width_-24,45,value);
  std::snprintf(value,sizeof(value),"%02d:%02d",hour(active),minute(active));
  place_label(overlay_clock_,8,overlay_height_*29/100,overlay_width_-16,overlay_height_*21/100,value);
  std::snprintf(value,sizeof(value),"%s %d/3%s",text("snooze"),a_[active].snoozes_used,
                a_[active].audio_expired?"  |  Ton beendet":"");
  place_label(overlay_state_,8,overlay_height_*55/100,overlay_width_-16,48,value);
  const bool can_snooze=!a_[active].test && a_[active].snoozes_used<3;
  lv_obj_t *button=lv_obj_get_parent(overlay_snooze_);
  if (button) {
    if (can_snooze) lv_obj_remove_flag(button,LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(button,LV_OBJ_FLAG_HIDDEN);
  }
  lv_obj_move_foreground(alarm_overlay_);
}

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
  for (int d=0;d<7;++d) if (v.find(DAY_ABBR[d])!=std::string::npos) mask |= 1u<<d;
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
  for (int k=0;k<7;++k) if (mask&(1u<<k)) { if (!value.empty()) value+=','; value+=DAY_ABBR[k]; }
  auto call=a_[i].days->make_call(); call.set_value(value); call.perform(); tessera::refresh();
}
const char *AlarmClock::alarm_name(int i) const {
  if (i < 0 || i >= COUNT || !a_[i].name || a_[i].name->state.empty()) return "Wecker";
  return a_[i].name->state.c_str();
}
void AlarmClock::set_alarm_name(int i, const std::string &name) {
  if (i < 0 || i >= COUNT || !a_[i].name || name.empty()) return;
  auto call=a_[i].name->make_call();
  call.set_value(name.substr(0,24));
  call.perform();
  tessera::refresh();
}
bool AlarmClock::occupied(int i) const {
  return i>=0 && i<COUNT && ((occupied_mask_ & (1u<<i)) != 0);
}
int AlarmClock::occupied_count() const {
  int count=0;
  for (int i=0;i<COUNT;++i) if (occupied(i)) ++count;
  return count;
}
void AlarmClock::publish_occupied_() {
  if (!occupied_sensor_ || last_published_mask_==occupied_mask_) return;
  char bits[COUNT+1];
  for (int i=0;i<COUNT;++i) bits[i]=occupied(i)?'1':'0';
  bits[COUNT]=0;
  occupied_sensor_->publish_state(bits);
  last_published_mask_=occupied_mask_;
}
void AlarmClock::set_occupied(int i,bool yes) {
  if (i<0 || i>=COUNT) return;
  uint8_t next=occupied_mask_;
  if (yes) next|=1u<<i; else next &= ~(1u<<i);
  if(next==occupied_mask_) return;
  occupied_mask_=next;
  uint32_t store=occupied_mask_;
  occupied_preference_.save(&store);
  publish_occupied_();
  tessera::refresh();
}
void AlarmClock::handle_action(const std::string &value) {
  // Home Assistant: add:1..5 or remove:1..5. No arbitrary code or parser.
  if (value.size()!=5 && value.size()!=8) return;
  if (value.rfind("add:",0)==0 && value.size()==5 && value[4]>='1' && value[4]<='5')
    set_occupied(value[4]-'1',true);
  if (value.rfind("remove:",0)==0 && value.size()==8 && value[7]>='1' && value[7]<='5') {
    int i=value[7]-'1'; set_active(i,false); set_occupied(i,false);
  }
}
void AlarmClock::preview_tone(int tune) {
  if (!player_ || any_ringing()) return;
  player_->stop();
  player_->set_gain(0.05f);
  player_->play(SONGS[clamp_int(tune,0,4)]);
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
  s.active=true; s.test=test; s.started_ms=millis(); s.snooze_pending=false; s.audio_expired=false;
  if (!any_ringing() || audio_owner_<0 || !a_[audio_owner_].active) audio_owner_=i;
  tessera::wake(this);
  update_alarm_overlay_();
  last_song_ms_=0;
  play_current_(millis());
  tessera::refresh();
}
void AlarmClock::start_test() { a_[0].snoozes_used=0; start_alarm_(0,true); }
void AlarmClock::stop_alarm(int i) {
  if (i<0 || i>=COUNT) return;
  Slot &s=a_[i]; s.active=false; s.snooze_pending=false; s.test=false; s.snoozes_used=0;
  refresh_audio_owner_(millis());
  update_alarm_overlay_();
  tessera::refresh();
}
void AlarmClock::stop_all() { for(int i=0;i<COUNT;++i) {a_[i].active=false; a_[i].snooze_pending=false; a_[i].snoozes_used=0;} audio_owner_=-1; if(player_) player_->stop(); update_alarm_overlay_(); tessera::refresh(); }
void AlarmClock::snooze_alarm(int i) {
  if(i<0 || i>=COUNT || !a_[i].active) return;
  Slot &s=a_[i];
  if(s.test || s.snoozes_used>=3) { stop_alarm(i); return; }
  s.active=false; s.snooze_pending=true; ++s.snoozes_used;
  s.snooze_due_ms=millis()+static_cast<uint32_t>(snooze_minutes(i))*60000u;
  refresh_audio_owner_(millis());
  update_alarm_overlay_();
  tessera::refresh();
}
void AlarmClock::refresh_audio_owner_(uint32_t now_ms) {
  int next=-1;
  if (audio_owner_>=0 && audio_owner_<COUNT && a_[audio_owner_].active && !a_[audio_owner_].audio_expired)
    next=audio_owner_;
  else for (int i=0;i<COUNT;++i) if (a_[i].active && !a_[i].audio_expired) { next=i; break; }
  if (next==audio_owner_) return;
  audio_owner_=next;
  if(player_) player_->stop();
  last_song_ms_=0;
  if(next>=0) play_current_(now_ms);
}
void AlarmClock::play_current_(uint32_t now_ms) {
  if(!player_ || audio_owner_<0) return;
  const Slot &s=a_[audio_owner_];
  if (s.audio_expired) return;
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
  // Preserve enabled slots and migrate configured-but-disabled slots from 0.1.0 once.
  if(migration_pending_ && now_ms >= 12000u) {
    for(int i=0;i<COUNT;++i) {
      if(enabled(i) || hour(i)!=(7+i) || minute(i)!=0 || mode(i)!=0 ||
         tune(i)!=1 || volume(i)!=60 || snooze_minutes(i)!=5 || weekdays(i)!=31)
        set_occupied(i,true);
    }
    migration_pending_=false;
    uint32_t stored=occupied_mask_;
    occupied_preference_.save(&stored);
  }
  for(int i=0;i<COUNT;++i) if(enabled(i) && !occupied(i)) set_occupied(i,true);
  publish_occupied_();
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
    // Audio ends after 5 minutes (15 seconds for a test), but the alarm stays
    // pending until a conscious Stop or Snooze action acknowledges it.
    if(s.active && !s.audio_expired &&
       static_cast<uint32_t>(now_ms-s.started_ms)>=(s.test?15000u:300000u)) {
      s.audio_expired=true;
      refresh_audio_owner_(now_ms);
    }
  }
  if(audio_owner_<0 || !a_[audio_owner_].active || a_[audio_owner_].audio_expired)
    refresh_audio_owner_(now_ms);
  if(audio_owner_>=0) play_current_(now_ms);
  if (any_ringing() && (alarm_overlay_==nullptr ||
       static_cast<uint32_t>(now_ms-last_overlay_refresh_ms_)>=400u)) {
    update_alarm_overlay_();
    last_overlay_refresh_ms_=now_ms;
  }
}
bool AlarmClock::settings(tessera::SettingsPage &page) {
  page.icon="\U000F0020";
  page.card(text("list_title"),"\U000F0020","overview");
  page.action(text("test"),"\U000F0387",[this](){start_test();});
  page.action(text("stop_all"),"\U000F0020",[this](){stop_all();});
  return true;
}
void AlarmClock::setup() {
  occupied_preference_=global_preferences->make_preference<uint32_t>(0xAC4200u);
  uint32_t stored=0;
  if(occupied_preference_.load(&stored)) occupied_mask_=static_cast<uint8_t>(stored&0x1Fu);
  else migration_pending_=true;
  publish_occupied_();
  for(int i=0;i<COUNT;++i) {
    a_[i].preference=global_preferences->make_preference<uint32_t>(0xAC4100u+static_cast<uint32_t>(i));
    a_[i].preference.load(&a_[i].last_fired_date);
  }
  add_tile("next_alarm",[this](){return new NextAlarmTile(this);});
  add_card("overview",[this](){return new AlarmOverview(this);},true);
}
} // namespace esphome::alarmclock
