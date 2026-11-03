// A real 32-bit millis() rollover during an alarm must not be confused with
// a new alarm timestamp slightly ahead of an older scheduler tick.
#include "alarmclock.h"
#include <cassert>
namespace esphome {GlobalPrefs backing_prefs; GlobalPrefs *global_preferences=&backing_prefs;}
using namespace esphome;
using namespace esphome::alarmclock;
int main() {
  AlarmClock clock;
  rtttl::Rtttl player;
  speaker::Speaker output;
  clock.set_tone_player(&player);
  clock.set_speaker(&output);
  mock_time_ms=0xFFFFFFF0u;
  clock.start_test();
  assert(clock.any_ringing());
  assert(player.playing);
  mock_time_ms=0x00000010u;
  clock.on_interval(0x0000000Fu);
  assert(clock.any_ringing());
  assert(player.playing);
  mock_time_ms=75000u;
  clock.on_interval(75000u);
  assert(clock.any_ringing()); // Must require explicit Stop even after audio expires.
  assert(!player.playing);
  clock.stop_all();
  assert(!clock.any_ringing());
  return 0;
}
