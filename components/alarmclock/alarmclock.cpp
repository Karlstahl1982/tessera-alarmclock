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
      int row = (y - list_top_()) / std::max(1, list_rh_());
      if (y >= list_top_() && row >= 0 && row < listed_) {
        const int slot = visible_[row];
        if (x > w_ * 4 / 5) p_->set_active(slot, !p_->enabled(slot));
        else begin_edit_(slot);
      } else if (y >= list_top_() && row == listed_ && listed_ < AlarmClock::COUNT) {
        begin_new_();
      }
    } else {
      const int btn_y = h_ - ui::px(48);
      if (y >= btn_y) {
        if (x < w_/2) previous_();
        else next_step_();
      } else if (screen_ == TYPE) {
        const int row = (y-ui::px(62)) / std::max(1, option_rh_());
        if (row >= 0 && row <= 2) draft_.mode = row;
      } else if (screen_ == DAYS) {
        const int col = (x * 4) / std::max(1,w_);
        const int row = (y-ui::px(70))/std::max(1, option_rh_());
        const int day = row*4 + col;
        if (day>=0 && day<7) draft_.days ^= 1u<<day;
      } else if (screen_ == SOUND) {
        if (y>=h_-ui::px(94) && y<h_-ui::px(52)) p_->preview_tone(draft_.tone);
        else {
          const int rh = std::max(1,std::min(option_rh_(),(h_-ui::px(157))/5));
          const int row = (y-ui::px(58))/rh;
          if (y>=ui::px(58) && row>=0 && row<5) draft_.tone=row;
        }
      } else if (screen_ == OPTIONS) {
        const int row=(y-ui::px(66))/std::max(1,option_rh_());
        if (row==0) cycle_name_();
        else if (row==1) draft_.volume=clamp_int(draft_.volume+(x<w_/2?-10:10),10,100);
        else if (row==2) draft_.snooze=draft_.snooze==5?10:5;
        else if (row==3) draft_.active=!draft_.active;
      } else if (screen_==REVIEW && !new_ && y>=h_-ui::px(101) && y<h_-ui::px(52)) {
        screen_=CONFIRM_DELETE;
      }
    }
    render_();
  }
 private:
  enum Screen { LIST, TYPE, TIME, DATE, DAYS, SOUND, OPTIONS, REVIEW, CONFIRM_DELETE };
  struct Draft {
    int hour{7}, minute{0}, year{2026}, month{11}, day{4};
    int mode{0}, tone{1}, volume{60}, snooze{5};
    uint8_t days{31};
    bool active{true};
    std::string name{"Wecker"};
  } draft_{};
  int list_top_() const { return ui::px(52); }
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
    screen_=TYPE;
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
  void hide_all_() {
    for(auto *r:rows_) toggle_hidden(r,false);
    for(auto *d:days_) toggle_hidden(d,false);
    for(auto *r:{hour_,minute_,day_,month_,year_}) toggle_hidden(r,false);
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
      heading_(p_->text("list_title"),p_->text("list_subtitle"));
      listed_=0;
      for(int i=0;i<AlarmClock::COUNT;++i) if(p_->occupied(i)) visible_[listed_++]=i;
      for(int row=0;row<listed_;++row) {
        const int i=visible_[row];
        std::snprintf(b,sizeof(b),"%02d:%02d   %s   %s",p_->hour(i),p_->minute(i),p_->alarm_name(i),p_->enabled(i)?"AN":"AUS");
        chip_(rows_[row],ui::px(2),list_top_()+row*list_rh_(),w_-ui::px(4),list_rh_()-ui::px(3),b,false);
        toggle_hidden(rows_[row],true);
      }
      if (listed_<AlarmClock::COUNT) {
        chip_(rows_[listed_],ui::px(2),list_top_()+listed_*list_rh_(),w_-ui::px(4),list_rh_()-ui::px(3),p_->text("add_alarm"),true);
        toggle_hidden(rows_[listed_],true);
      }
      if (!listed_) place_label(subtitle_,0,h_/2,w_,ui::px(35),p_->text("empty_list"));
      place_label(rows_[7],0,h_-ui::px(37),w_,ui::px(32),p_->time_valid()?p_->text("list_hint"):p_->text("clock_missing"));
      toggle_hidden(rows_[7],true);
      return;
    }
    const char *step="";
    switch(screen_) {
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
    std::snprintf(b,sizeof(b),"%s · %s",new_?p_->text("add_alarm"):p_->text("edit_alarm"),step);
    heading_(b,draft_.name.c_str());
    footer_((screen_==REVIEW)?p_->text("save"):(screen_==CONFIRM_DELETE)?p_->text("delete_alarm"):p_->text("continue"));
    if(screen_==TYPE) {
      for(int i=0;i<3;++i) {
        const char *mode=i==0?p_->text("mode_next"):i==1?p_->text("mode_date"):p_->text("mode_weekly");
        chip_(rows_[i],ui::px(4),ui::px(62)+i*option_rh_(),w_-ui::px(8),option_rh_()-ui::px(5),mode,i==draft_.mode);
        toggle_hidden(rows_[i],true);
      }
    } else if (screen_==TIME || screen_==DATE) {
      const bool is_time=screen_==TIME;
      const int count=is_time?2:3;
      const int rw=(w_-ui::px(20))/count;
      const int rh=std::min(ui::px(163),std::max(ui::px(90),h_-ui::px(152)));
      const int ry=std::max(ui::px(62), (h_-rh)/2-ui::px(8));
      for(int j=0;j<count;++j) {
        lv_obj_t *r=is_time?(j==0?hour_:minute_):(j==0?day_:j==1?month_:year_);
        lv_obj_set_pos(r,ui::px(8)+j*rw,ry);
        lv_obj_set_size(r,rw-ui::px(4),rh);
        color_roller(r);
        toggle_hidden(r,true);
      }
    } else if(screen_==DAYS) {
      for(int i=0;i<7;++i) {
        const int col=i%4,row=i/4;
        chip_(days_[i],col*w_/4+ui::px(3),ui::px(78)+row*option_rh_(),w_/4-ui::px(6),option_rh_()-ui::px(6),DAY_ABBR[i],(draft_.days&(1<<i))!=0);
        toggle_hidden(days_[i],true);
      }
    } else if(screen_==SOUND) {
      const int rh=std::max(ui::px(24),std::min(option_rh_(),(h_-ui::px(157))/5));
      for(int i=0;i<5;++i) {
        const char *tone=p_->text(i==0?"tone_classic":i==1?"tone_gentle":i==2?"tone_digital":i==3?"tone_sunrise":"tone_urgent");
        chip_(rows_[i],ui::px(3),ui::px(58)+i*rh,w_-ui::px(6),rh-ui::px(3),tone,draft_.tone==i);
        toggle_hidden(rows_[i],true);
      }
      chip_(rows_[6],ui::px(25),h_-ui::px(96),w_-ui::px(50),ui::px(40),p_->text("preview_tone"));
      toggle_hidden(rows_[6],true);
    } else if(screen_==OPTIONS) {
      std::snprintf(b,sizeof(b),"%s: %s",p_->text("name"),draft_.name.c_str());
      chip_(rows_[0],ui::px(3),ui::px(66),w_-ui::px(6),option_rh_()-ui::px(4),b);
      std::snprintf(b,sizeof(b),"−     %s: %d%%     +",p_->text("volume"),draft_.volume);
      chip_(rows_[1],ui::px(3),ui::px(66)+option_rh_(),w_-ui::px(6),option_rh_()-ui::px(4),b);
      std::snprintf(b,sizeof(b),"%s: %d min",p_->text("snooze"),draft_.snooze);
      chip_(rows_[2],ui::px(3),ui::px(66)+2*option_rh_(),w_-ui::px(6),option_rh_()-ui::px(4),b);
      std::snprintf(b,sizeof(b),"%s: %s",p_->text("enabled"),draft_.active?p_->text("on"):p_->text("off"));
      chip_(rows_[3],ui::px(3),ui::px(66)+3*option_rh_(),w_-ui::px(6),option_rh_()-ui::px(4),b,draft_.active);
      for(int i=0;i<4;++i) toggle_hidden(rows_[i],true);
    } else if(screen_==REVIEW) {
      std::snprintf(b,sizeof(b),"%02d:%02d  ·  %s",draft_.hour,draft_.minute,draft_.name.c_str());
      chip_(rows_[0],ui::px(2),ui::px(62),w_-ui::px(4),ui::px(43),b,true);
      const char *mode=draft_.mode==0?p_->text("mode_next"):draft_.mode==1?p_->text("mode_date"):p_->text("mode_weekly");
      chip_(rows_[1],ui::px(2),ui::px(109),w_-ui::px(4),ui::px(39),mode);
      std::snprintf(b,sizeof(b),"%s: %d%%  |  %s: %d min",p_->text("volume"),draft_.volume,p_->text("snooze"),draft_.snooze);
      chip_(rows_[2],ui::px(2),ui::px(152),w_-ui::px(4),ui::px(39),b);
      std::snprintf(b,sizeof(b),"%s: %s",p_->text("enabled"),draft_.active?p_->text("on"):p_->text("off"));
      chip_(rows_[3],ui::px(2),ui::px(195),w_-ui::px(4),ui::px(39),b);
      for(int i=0;i<4;++i) toggle_hidden(rows_[i],true);
      if(!new_) {chip_(rows_[6],ui::px(20),h_-ui::px(99),w_-ui::px(40),ui::px(39),p_->text("delete_alarm"));toggle_hidden(rows_[6],true);}
    } else if(screen_==CONFIRM_DELETE) {
      place_label(rows_[0],ui::px(8),h_/3,w_-ui::px(16),ui::px(70),p_->text("delete_confirm"));
      toggle_hidden(rows_[0],true);
    }
  }
  AlarmClock *p_{};
  Screen screen_{LIST};
  bool new_{false};
  int selected_{-1}, w_{0}, h_{0}, listed_{0};
  int visible_[AlarmClock::COUNT]{};
  lv_obj_t *title_{},*subtitle_{},*rows_[9]{},*days_[7]{},*back_{},*next_{};
  lv_obj_t *hour_{},*minute_{},*day_{},*month_{},*year_{};
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
