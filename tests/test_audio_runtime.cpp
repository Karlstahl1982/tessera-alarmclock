#include "alarmclock.h"
#include <cassert>
#include <cmath>
namespace esphome {GlobalPrefs backing_prefs; GlobalPrefs *global_preferences=&backing_prefs;}
using namespace esphome;
using namespace esphome::alarmclock;
int main() {
  AlarmClock clock;
  rtttl::Rtttl player;
  speaker::Speaker output;
  number::Number volume;
  volume.state=100.0f;
  clock.set_tone_player(&player);
  clock.set_speaker(&output);
  clock.set_volume(0,&volume);
  mock_time_ms=42;
  clock.start_test();
  assert(player.plays==1 && player.playing);
  assert(std::fabs(player.gain-VolumeRamp::digital_gain(100,0))<0.00001f);
  mock_time_ms=142;
  output.running=true;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==1);
  assert(std::fabs(output.last_volume-VolumeRamp::digital_gain(100,100))<0.00001f);
  mock_time_ms=15041;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==1); // 14.999s: not yet
  mock_time_ms=15042;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==2); // +5 points at exactly 15s
  assert(std::fabs(output.last_volume-VolumeRamp::digital_gain(100,15000))<0.00001f);
  mock_time_ms=29999;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==2);
  mock_time_ms=30042;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==3);
  assert(std::fabs(output.last_volume-VolumeRamp::digital_gain(100,30000))<0.00001f);
  mock_time_ms=60042;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==4);
  assert(std::fabs(output.last_volume-VolumeRamp::digital_gain(100,60000))<0.00001f);
  clock.stop_all();
  assert(player.stops>=1);
  return 0;
}
