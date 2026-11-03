#include "alarmclock.h"
#include <cassert>
namespace esphome {GlobalPrefs backing_prefs;GlobalPrefs *global_preferences=&backing_prefs;}
using namespace esphome;
using namespace esphome::alarmclock;
int main(){
  AlarmClock clock;
  rtttl::Rtttl player;
  speaker::Speaker speaker;
  switch_::Switch once0, once1;
  datetime::TimeEntity tm0, tm1;
  once0.state=true;
  once1.state=true;
  clock.set_tone_player(&player);clock.set_speaker(&speaker);
  clock.set_enabled(0,&once0);clock.set_time(0,&tm0);
  clock.set_enabled(1,&once1);clock.set_time(1,&tm1);
  mock_time_ms=200000;
  clock.on_interval(mock_time_ms);
  assert(clock.ringing(0));
  assert(clock.ringing(1));
  clock.snooze_alarm(0);
  clock.snooze_alarm(1);
  assert(clock.snoozing(0));
  assert(clock.snoozing(1));
  assert(clock.next_alarm()==0); // tie: first slot wins
  clock.stop_alarm(0); // UI cancels only the selected snooze
  assert(!clock.snoozing(0));
  assert(!clock.ringing(0));
  assert(clock.snoozing(1)); // unrelated alarm preserved
  assert(clock.next_alarm()==1);
  clock.stop_alarm(1);
  assert(!clock.snoozing(1));
  assert(clock.next_alarm()!=0 || !clock.snoozing(0));
  return 0;
}
