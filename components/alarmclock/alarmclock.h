#pragma once
#include <cstdint>
#include <cmath>
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"
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

  void set_hour(number::Number *value) { hour_ = value; }
  void set_minute(number::Number *value) { minute_ = value; }
  void set_enabled(switch_::Switch *value) { enabled_ = value; }
  void set_tone(rtttl::Rtttl *value) { tone_ = value; }

  int hour() const;
  int minute() const;
  bool enabled() const;
  bool ringing() const { return ringing_; }
  bool time_valid() const { return tessera::epoch() != 0; }
  void toggle();
  void start_test();
  void stop_alarm();

 private:
  void start_alarm_(bool test);
  number::Number *hour_{nullptr};
  number::Number *minute_{nullptr};
  switch_::Switch *enabled_{nullptr};
  rtttl::Rtttl *tone_{nullptr};
  uint32_t last_seen_minute_{0};
  uint32_t alarm_started_ms_{0};
  uint32_t last_play_ms_{0};
  bool ringing_{false};
  bool test_{false};
};

}  // namespace esphome::alarmclock
