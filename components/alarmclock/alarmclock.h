#pragma once
#include <cstdint>
#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include "esphome/components/datetime/time_entity.h"
#include "esphome/components/datetime/date_entity.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/select/select.h"
#include "esphome/components/text/text.h"
#include "esphome/components/number/number.h"
#include "esphome/components/rtttl/rtttl.h"
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::alarmclock {

class AlarmClock final : public Component, public tessera::Plugin {
 public:
  static constexpr int COUNT = 5;
  void setup() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void on_interval(uint32_t now_ms) override;
  bool settings(tessera::SettingsPage &page) override;

  void set_tone_player(rtttl::Rtttl *p) { player_ = p; }
  void set_time(int i, datetime::TimeEntity *p) { a_[i].time = p; }
  void set_date(int i, datetime::DateEntity *p) { a_[i].date = p; }
  void set_enabled(int i, switch_::Switch *p) { a_[i].enabled = p; }
  void set_mode(int i, select::Select *p) { a_[i].mode = p; }
  void set_tone(int i, select::Select *p) { a_[i].tone = p; }
  void set_snooze(int i, select::Select *p) { a_[i].snooze = p; }
  void set_volume(int i, number::Number *p) { a_[i].volume = p; }
  void set_days(int i, text::Text *p) { a_[i].days = p; }

  int hour(int i) const;
  int minute(int i) const;
  int year(int i) const;
  int month(int i) const;
  int day(int i) const;
  int mode(int i) const;
  int tune(int i) const;
  int snooze_minutes(int i) const;
  int volume(int i) const;
  bool enabled(int i) const;
  bool weekday_enabled(int i, int mon_based) const;
  uint8_t weekdays(int i) const;
  void set_alarm_time(int i, int h, int m);
  void set_alarm_date(int i, int y, int m, int d);
  void set_alarm_mode(int i, int mode);
  void set_alarm_tone(int i, int tone);
  void set_alarm_volume(int i, int volume);
  void set_snooze_minutes(int i, int minutes);
  void set_weekday(int i, int mon_based, bool on);
  void set_active(int i, bool on);
  void start_test();
  void stop_all();
  void stop_alarm(int i);
  void snooze_alarm(int i);
  bool ringing(int i) const;
  bool any_ringing() const;
  int first_ringing() const;
  int next_alarm() const;
  bool time_valid() const { return tessera::epoch() != 0; }
  const char *mode_label(int i) const;
  const char *tone_label(int i) const;
  int snoozes_used(int i) const { return a_[i].snoozes_used; }

 private:
  struct Slot {
    datetime::TimeEntity *time{nullptr};
    datetime::DateEntity *date{nullptr};
    switch_::Switch *enabled{nullptr};
    select::Select *mode{nullptr};
    select::Select *tone{nullptr};
    select::Select *snooze{nullptr};
    number::Number *volume{nullptr};
    text::Text *days{nullptr};
    uint32_t last_fired_date{0};
    ESPPreferenceObject preference{};
    uint32_t started_ms{0};
    uint32_t snooze_due_ms{0};
    uint8_t snoozes_used{0};
    bool active{false};
    bool snooze_pending{false};
    bool test{false};
  } a_[COUNT];
  void start_alarm_(int i, bool test);
  void play_current_(uint32_t now_ms);
  void refresh_audio_owner_(uint32_t now_ms);
  bool matches_(int i, uint32_t candidate_epoch, uint32_t &date_key) const;
  rtttl::Rtttl *player_{nullptr};
  uint32_t seen_minute_{0};
  uint32_t last_song_ms_{0};
  int audio_owner_{-1};
};

}  // namespace esphome::alarmclock
