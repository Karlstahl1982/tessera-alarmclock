#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <functional>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <utility>
struct lv_obj_t {};
struct lv_display_t {};
struct lv_event_t {};
using lv_color_t=int;
using lv_event_cb_t=void(*)(lv_event_t*);
inline lv_color_t lv_color_hex(int c){return c;}
inline lv_display_t *lv_display_get_default(){static lv_display_t d;return &d;}
inline int lv_display_get_horizontal_resolution(lv_display_t*){return 480;}
inline int lv_display_get_vertical_resolution(lv_display_t*){return 480;}
inline lv_obj_t *lv_layer_sys(){return nullptr;}
inline lv_obj_t *lv_obj_create(lv_obj_t*){return nullptr;}
inline void lv_obj_delete(lv_obj_t*){}
inline void lv_obj_move_foreground(lv_obj_t*){}
inline lv_obj_t *lv_obj_get_parent(lv_obj_t*){return nullptr;}
inline void *lv_event_get_user_data(lv_event_t*){return nullptr;}
inline void lv_obj_add_event_cb(lv_obj_t*,lv_event_cb_t,int,void*){}
enum {LV_TEXT_ALIGN_CENTER,LV_OBJ_FLAG_HIDDEN,LV_PART_MAIN,LV_PART_SELECTED,LV_ROLLER_MODE_INFINITE,LV_ROLLER_MODE_NORMAL,LV_ANIM_OFF,LV_OPA_COVER,LV_OBJ_FLAG_CLICKABLE,LV_OBJ_FLAG_SCROLLABLE,LV_EVENT_CLICKED};
inline void lv_obj_set_pos(lv_obj_t*,int,int){}
inline void lv_obj_set_size(lv_obj_t*,int,int){}
inline void lv_obj_set_width(lv_obj_t*,int){}
inline void lv_obj_set_style_text_align(lv_obj_t*,int,int){}
inline void lv_obj_set_style_bg_color(lv_obj_t*,int,int){}
inline void lv_obj_set_style_text_color(lv_obj_t*,int,int){}
inline void lv_obj_set_style_text_font(lv_obj_t*,void*,int){}
inline void lv_obj_set_style_border_width(lv_obj_t*,int,int){}
inline void lv_obj_set_style_border_color(lv_obj_t*,int,int){}
inline void lv_obj_set_style_pad_all(lv_obj_t*,int,int){}
inline void lv_obj_set_style_radius(lv_obj_t*,int,int){}
inline void lv_obj_set_style_bg_opa(lv_obj_t*,int,int){}
inline void lv_obj_set_style_pad_hor(lv_obj_t*,int,int){}
inline void lv_obj_remove_flag(lv_obj_t*,int){}
inline void lv_obj_add_flag(lv_obj_t*,int){}
inline lv_obj_t *lv_roller_create(lv_obj_t*){return nullptr;}
inline void lv_roller_set_options(lv_obj_t*,const char*,int){}
inline void lv_roller_set_visible_row_count(lv_obj_t*,int){}
inline void lv_roller_set_selected(lv_obj_t*,int,int){}
inline int lv_roller_get_selected(lv_obj_t*){return 0;}
inline uint32_t millis(){return 42;}
namespace theme { enum R {CARD,INK,ACCENT,ON_ACCENT,MUTED}; }
namespace esphome {
namespace setup_priority { static constexpr float DATA=10; }
class Component { public: virtual ~Component(){}; virtual void setup(){}; virtual float get_setup_priority()const{return 0;}};
struct ESPPreferenceObject {template<class T> bool load(T*){return false;} template<class T> void save(T*){}};
struct GlobalPrefs {template<class T> ESPPreferenceObject make_preference(uint32_t){return {};}};
extern GlobalPrefs *global_preferences;
namespace datetime {
 struct TimeCall {void set_time(uint8_t,uint8_t,int){};void perform(){}};
 struct TimeEntity {int hour=7,minute=0;bool has_state()const{return true;} TimeCall make_call(){return {};}};
 struct DateCall {void set_date(int,int,int){};void perform(){}};
 struct DateEntity {int year=2026,month=11,day=4;bool has_state()const{return true;} DateCall make_call(){return {};}};
}
namespace switch_ {struct Switch {bool state=false;void turn_on(){};void turn_off(){}};}
namespace select {struct SelectCall{void set_option(const char*){};void perform(){}};struct Select {std::string current_option()const{return "Next";} SelectCall make_call(){return {};}};}
namespace text {struct TextCall {void set_value(const std::string&){};void perform(){}};struct Text {std::string state="Wecker 1";TextCall make_call(){return {};}};}
namespace number {struct NumberCall{void set_value(int){};void perform(){}};struct Number {float state=60.0;NumberCall make_call(){return {};}};}
namespace text_sensor {struct TextSensor{void publish_state(const char*){}};}
namespace rtttl {struct Rtttl {void set_gain(float){}; bool is_playing()const{return false;}void play(const char*){};void stop(){}};}
}
namespace tessera {
enum class Font {TITLE,VALUE,BODY,BODY_LARGE,HEADLINE};
struct TileContext{lv_obj_t* parent=nullptr;int width=480,height=360,tile=0;};
struct CardContext{lv_obj_t* parent=nullptr;int width=480,height=360,tile=0;};
struct PopupContext{lv_obj_t* parent=nullptr;int width=480,height=360;};
struct Tap{int x=0,y=0;};
struct Time {int hour=7,minute=0,year=2026,month=11,day=3,weekday=2;};
enum class PopupWidth {WIDE};
struct Tile { virtual ~Tile(){};virtual void create(const TileContext&){};virtual void on_tick(uint32_t){};virtual void on_theme(){};virtual void on_tap(){};};
struct Card {virtual ~Card(){};virtual void open(const CardContext&){};virtual void on_tick(uint32_t){};virtual void on_theme(){};virtual void on_tap(){};virtual bool on_back(){return false;}};
struct Popup {virtual ~Popup(){};virtual void open(const PopupContext&){};virtual void on_tick(uint32_t){};virtual void on_theme(){};virtual void on_tap(){};};
struct SettingsPage {std::string icon; void card(const char*,const char*,const char*){};template<class F>void action(const char*,const char*,F){}};
struct Plugin {virtual ~Plugin(){};virtual void on_interval(uint32_t){};virtual bool settings(SettingsPage&){return false;}const char* text(const char*)const{return "A";} const char* plugin_id()const{return "alarmclock";}
 template<class F>void add_tile(const char*,F){};template<class F>void add_card(const char*,F,bool){};};
inline uint32_t epoch(){return 123456789;}
inline Time local_time(uint32_t){return {};}
inline void open_card(const char*,const char*,const char*,int){}
inline void wake(Plugin*){}
inline void refresh(){}
inline bool popup_shown(Plugin*){return false;}
inline void close_popup(Plugin*){}
template<class F>void show_popup(Plugin*,F,PopupWidth){}
inline Tap tap_point(){return {};}
namespace ui {
 inline int color(theme::R){return 0;}
 inline void* font(Font){return nullptr;}
 inline int px(int x){return x;}
 inline int mm(int x){return x*4;}
 inline int line_height(Font){return 32;}
 inline lv_obj_t* label(lv_obj_t*,Font,theme::R){return nullptr;}
 inline void set_text(lv_obj_t*,const char*){}
 inline void set_text(lv_obj_t*,const std::string&){}
 inline void set_color(lv_obj_t*,theme::R){}
}
}
