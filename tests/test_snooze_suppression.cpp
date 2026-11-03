#include "alarmclock.h"
#include <cassert>
namespace esphome {GlobalPrefs backing_prefs;GlobalPrefs *global_preferences=&backing_prefs;}
using namespace esphome;
using namespace esphome::alarmclock;
int main(){
 AlarmClock clock;
 rtttl::Rtttl player;
 speaker::Speaker speaker;
 switch_::Switch once;
 datetime::TimeEntity tm;
 switch_::Switch suppression;
 once.state=true;tm.hour=7;tm.minute=0;
 clock.set_tone_player(&player);clock.set_speaker(&speaker);
 clock.set_enabled(0,&once);clock.set_time(0,&tm);
 clock.set_suppress_switch(&suppression);
 mock_time_ms=200000;
 clock.on_interval(200000); // schedules first occurrence
 assert(clock.ringing(0));
 clock.snooze_alarm(0);
 once.state=false;  // one-off disabled automatically after its first ring
 assert(clock.snoozing(0));
 assert(clock.next_alarm()==0);  // still the next alarm even while disabled
 assert(clock.snooze_remaining_seconds(0)>0);
 mock_time_ms+=60000;
 assert(clock.snoozing(0));
 assert(clock.next_alarm()==0);
 suppression.state=true;
 clock.set_suppressed(true);
 assert(!clock.snoozing(0));
 assert(!clock.any_ringing());
 return 0;
}
