#pragma once
#include <cstdint>
#include "esphome/core/component.h"
#include "esphome/components/datetime/time_entity.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/rtttl/rtttl.h"
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::alarmclock {

class AlarmClock : public Component, public tessera::Plugin {
 public:
  void setup() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void on_interval(uint32_t now_ms) override;
  bool settings(tessera::SettingsPage &page) override;

  void set_alarm_time(datetime::TimeEntity *value) { alarm_time_ = value; }
  void set_enabled(switch_::Switch *value) { enabled_ = value; }
  void set_tone(rtttl::Rtttl *value) { tone_ = value; }

  int hour() const;
  int minute() const;
  void adjust_hour(int delta);
  void adjust_minute(int delta);
  bool enabled() const;
  bool ringing() const { return ringing_; }
  bool time_valid() const { return tessera::epoch() != 0; }
  void toggle();
  void start_test();
  void stop_alarm();

 private:
  void start_alarm_(bool test);
  datetime::TimeEntity *alarm_time_{nullptr};
  switch_::Switch *enabled_{nullptr};
  rtttl::Rtttl *tone_{nullptr};
  uint32_t last_seen_minute_{0};
  uint32_t alarm_started_ms_{0};
  uint32_t last_play_ms_{0};
  bool ringing_{false};
  bool test_{false};
};

}  // namespace esphome::alarmclock
