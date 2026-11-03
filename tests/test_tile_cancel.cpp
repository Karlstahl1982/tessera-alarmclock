#include "alarmclock.h"
#include <cassert>
// The Tile class is defined in the C++ translation unit, not the header.
// Include that unit to exercise the real event/gesture routing.
#include "../components/alarmclock/alarmclock.cpp"
namespace esphome {GlobalPrefs backing_prefs; GlobalPrefs *global_preferences=&backing_prefs;}
using namespace esphome;
using namespace esphome::alarmclock;
int main() {
  AlarmClock clock;
  rtttl::Rtttl player;
  speaker::Speaker speaker;
  switch_::Switch enabled;
  datetime::TimeEntity time;
  enabled.state=true;
  time.hour=7; time.minute=0;
  clock.set_tone_player(&player);
  clock.set_speaker(&speaker);
  clock.set_enabled(0,&enabled);
  clock.set_time(0,&time);
  mock_time_ms=300000;
  clock.on_interval(mock_time_ms);
  assert(clock.ringing(0));
  clock.snooze_alarm(0);
  assert(clock.snoozing(0));
  NextAlarmTile tile(&clock);
  tessera::TileContext context;
  context.width=480;context.height=360;
  tile.create(context);
  const int initial_calls=tessera::mock_open_card_calls;
  tessera::mock_tap_point={130,170};
  tile.on_tap();
  assert(clock.snoozing(0));  // other parts of the tile do not cancel
  assert(tessera::mock_open_card_calls==initial_calls+1);
  tessera::mock_tap_point={450,330};
  tile.on_tap();
  assert(!clock.snoozing(0)); // the X ends the snooze lifecycle
  assert(!clock.any_ringing());
  assert(tessera::mock_open_card_calls==initial_calls+1); // no card opens
  tile.on_tap();
  assert(tessera::mock_open_card_calls==initial_calls+2); // normal edit mode
  return 0;
}
