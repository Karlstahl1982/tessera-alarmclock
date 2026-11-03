#include "volume_ramp.h"
#include <cassert>
#include <cmath>
using esphome::alarmclock::VolumeRamp;
int main() {
  assert(VolumeRamp::current_percent(60,0)==30);
  assert(VolumeRamp::current_percent(60,5999)==30);
  assert(VolumeRamp::current_percent(60,6000)==33);
  assert(VolumeRamp::current_percent(60,30000)==45);
  assert(VolumeRamp::current_percent(60,54000)==57);
  assert(VolumeRamp::current_percent(60,60000)==60);
  assert(VolumeRamp::current_percent(60,99000)==60);
  assert(VolumeRamp::seconds_to_next(0)==6);
  assert(VolumeRamp::seconds_to_next(1001)==5);
  assert(VolumeRamp::seconds_to_next(5999)==1);
  assert(VolumeRamp::seconds_to_next(6000)==6);
  assert(VolumeRamp::seconds_to_next(59999)==1);
  assert(VolumeRamp::seconds_to_next(60000)==0);
  assert(VolumeRamp::current_percent(10,0)==5);
  assert(VolumeRamp::current_percent(100,60000)==100);
  assert(std::fabs(VolumeRamp::digital_gain(60,0)-0.105f)<0.00001f);
  assert(std::fabs(VolumeRamp::digital_gain(60,60000)-0.21f)<0.00001f);
  return 0;
}
