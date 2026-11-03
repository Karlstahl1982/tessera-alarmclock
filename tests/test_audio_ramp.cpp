#include "volume_ramp.h"
#include <cassert>
#include <cmath>
using esphome::alarmclock::VolumeRamp;
int main() {
  assert(VolumeRamp::current_percent(60,0)==30);
  assert(VolumeRamp::current_percent(60,1000)==30);
  assert(VolumeRamp::current_percent(60,3000)==32);
  assert(VolumeRamp::current_percent(60,5999)==33);
  assert(VolumeRamp::current_percent(60,6000)==33);
  assert(VolumeRamp::current_percent(60,30000)==45);
  assert(VolumeRamp::current_percent(60,60000)==60);
  assert(VolumeRamp::current_percent(60,99000)==60);
  assert(VolumeRamp::seconds_to_next(0)==1);
  assert(VolumeRamp::seconds_to_next(1001)==1);
  assert(VolumeRamp::seconds_to_next(60000)==0);
  assert(VolumeRamp::seconds_remaining(0)==60);
  assert(VolumeRamp::seconds_remaining(6001)==54);
  assert(VolumeRamp::progress_percent(0)==0);
  assert(VolumeRamp::progress_percent(30000)==50);
  assert(VolumeRamp::progress_percent(60000)==100);
  assert(VolumeRamp::current_percent(10,0)==5);
  assert(VolumeRamp::current_percent(100,60000)==100);
  assert(std::fabs(VolumeRamp::digital_gain(60,0)-0.105f)<0.00001f);
  assert(std::fabs(VolumeRamp::digital_gain(60,1000)-0.10675f)<0.00001f);
  assert(std::fabs(VolumeRamp::digital_gain(60,60000)-0.21f)<0.00001f);
  return 0;
}
