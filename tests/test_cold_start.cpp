#include "alarmclock.h"
#include <cassert>
namespace esphome {GlobalPrefs backing_prefs; GlobalPrefs *global_preferences=&backing_prefs;}
using namespace esphome;
using namespace esphome::alarmclock;
int main() {
  alarmclock::AlarmClock clock;
  rtttl::Rtttl player;
  speaker::Speaker output;
  clock.set_tone_player(&player);
  clock.set_speaker(&output);
  mock_time_ms=42;
  clock.start_test();
  assert(player.plays==1);
  assert(player.stops==0);
  mock_time_ms=4042;
  clock.on_interval(mock_time_ms);
  assert(player.plays==2); // retry once if first I2S start stuck
  assert(player.stops==1);
  mock_time_ms=8042;
  clock.on_interval(mock_time_ms);
  assert(player.plays==3);
  mock_time_ms=12042;
  clock.on_interval(mock_time_ms);
  assert(player.plays==3); // never spin on repeated retries
  return 0;
}
