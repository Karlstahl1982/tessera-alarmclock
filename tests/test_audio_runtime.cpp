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
  alarmclock::AlarmClock clock;
  rtttl::Rtttl player;
  speaker::Speaker output;
  number::Number volume;
  volume.state=60.0f;
  clock.set_tone_player(&player);
  clock.set_speaker(&output);
  clock.set_volume(0,&volume);

  mock_time_ms=42;
  clock.start_test();
  // First alarm must request sound immediately; old implementation could
  // initialize at a near-silent 0.9% digital gain.
  assert(player.plays==1);
  assert(player.playing);
  assert(std::fabs(player.gain-0.105f)<0.00001f);

  mock_time_ms=142;
  output.running=true;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==1);
  assert(std::fabs(output.last_volume-0.105f)<0.00001f);

  // At a six-second boundary the I2S output must actually receive the gain.
  mock_time_ms=6042;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==2);
  assert(std::fabs(output.last_volume-0.1155f)<0.00001f);
  clock.on_interval(mock_time_ms+100);
  assert(output.volume_calls==2); // no repeated I2S calls within one stage

  mock_time_ms=60042;
  clock.on_interval(mock_time_ms);
  assert(output.volume_calls==3);
  assert(std::fabs(output.last_volume-0.21f)<0.00001f);
  clock.stop_all();
  assert(player.stops>=1);
  return 0;
}
