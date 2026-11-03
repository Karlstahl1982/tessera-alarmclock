// Regression: the scheduler captures a tick time; millis() advances between
// scheduling and starting the alarm. Unsigned subtraction previously caused
// an immediate 5-minute timeout ("Alarmton beendet") on the FIRST trigger.
#include "alarmclock.h"
#include <cassert>
namespace esphome { GlobalPrefs backing_prefs; GlobalPrefs *global_preferences=&backing_prefs; }
using namespace esphome;
using namespace esphome::alarmclock;
int main() {
  AlarmClock clock;
  rtttl::Rtttl player;
  speaker::Speaker speaker;
  switch_::Switch enable;
  datetime::TimeEntity time;
  enable.state=true;
  time.hour=7; time.minute=0;
  clock.set_tone_player(&player);
  clock.set_speaker(&speaker);
  clock.set_enabled(0,&enable);
  clock.set_time(0,&time);
  mock_time_ms=100001; // The engine captured its tick at 100000, one millisecond earlier.
  mock_millis_auto_advance=1;
  clock.on_interval(100000);
  mock_millis_auto_advance=0;
  assert(clock.ringing(0));
  assert(player.plays>=1);
  assert(player.playing);  // Previously stopped immediately after triggering!
  speaker.running=true;
  mock_time_ms=100075;
  clock.on_interval(100075);
  assert(player.playing);
  assert(speaker.volume_calls>=1);
  // An ordinary Stop/Snooze must not turn the 5-minute duration into a negative delta.
  clock.snooze_alarm(0);
  assert(!clock.ringing(0));
  mock_time_ms=400075;  // 5 minutes later
  clock.on_interval(400075);
  assert(clock.ringing(0));
  assert(player.playing);
  return 0;
}
