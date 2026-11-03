#include "alarmclock.h"
#include <cassert>
#include <cmath>
namespace esphome {
GlobalPrefs backing_prefs;
GlobalPrefs *global_preferences=&backing_prefs;
}
using namespace esphome;
using namespace esphome::alarmclock;
int main() {
  AlarmClock clock;
  rtttl::Rtttl player;
  speaker::Speaker output;
  number::Number volume;
  volume.state=60.0f;
  clock.set_tone_player(&player);
  clock.set_speaker(&output);
  clock.set_volume(0,&volume);
  mock_time_ms=42;
  clock.start_test();
  assert(player.plays==1);
  assert(player.playing);
  assert(std::fabs(player.gain-0.105f)<0.00001f);
  mock_time_ms=142;
  output.running=true;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==1);
  assert(std::fabs(output.last_volume-0.105175f)<0.00001f);
  mock_time_ms=1142;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==2);
  assert(std::fabs(output.last_volume-VolumeRamp::digital_gain(60,1100))<0.00001f);
  mock_time_ms=1642;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==2);
  mock_time_ms=6042;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==3);
  assert(std::fabs(output.last_volume-0.1155f)<0.00001f);
  mock_time_ms=60042;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==4);
  assert(std::fabs(output.last_volume-0.21f)<0.00001f);
  clock.stop_all();
  assert(player.stops>=1);
  return 0;
}
