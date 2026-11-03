#include "volume_ramp.h"
#include <cassert>
#include <cmath>
using esphome::alarmclock::VolumeRamp;
int main() {
  static_assert(VolumeRamp::STEP_MS==15000u);
  static_assert(VolumeRamp::START_PERCENT==5);
  static_assert(VolumeRamp::STEP_PERCENT==5);
  static_assert(VolumeRamp::DURATION_MS==285000u);
  assert(VolumeRamp::current_percent(100,0)==5);
  assert(VolumeRamp::current_percent(100,14999)==5);
  assert(VolumeRamp::current_percent(100,15000)==10);
  assert(VolumeRamp::current_percent(100,29999)==10);
  assert(VolumeRamp::current_percent(100,30000)==15);
  assert(VolumeRamp::current_percent(100,60000)==25);
  assert(VolumeRamp::current_percent(100,165000)==60);
  assert(VolumeRamp::current_percent(100,284999)==95);
  assert(VolumeRamp::current_percent(100,285000)==100);
  assert(VolumeRamp::current_percent(100,300000)==100);
  // Target cap remains valid for an individual alarm's maximum volume.
  assert(VolumeRamp::current_percent(60,0)==5);
  assert(VolumeRamp::current_percent(60,164999)==55);
  assert(VolumeRamp::current_percent(60,165000)==60);
  assert(VolumeRamp::current_percent(60,300000)==60);
  assert(VolumeRamp::duration_ms(60)==165000);
  assert(VolumeRamp::duration_ms(100)==285000);
  assert(VolumeRamp::duration_ms(10)==15000);
  assert(VolumeRamp::current_percent(10,15000)==10);
  assert(VolumeRamp::current_percent(77,225000)==77);
  assert(VolumeRamp::seconds_to_next(100,0)==15);
  assert(VolumeRamp::seconds_to_next(100,1)==15);
  assert(VolumeRamp::seconds_to_next(100,14999)==1);
  assert(VolumeRamp::seconds_to_next(100,15000)==15);
  assert(VolumeRamp::seconds_to_next(60,165000)==0);
  assert(VolumeRamp::seconds_remaining(100,0)==285);
  assert(VolumeRamp::seconds_remaining(60,15000)==150);
  assert(VolumeRamp::seconds_remaining(60,165000)==0);
  assert(VolumeRamp::progress_percent(100,0)==0);
  assert(VolumeRamp::progress_percent(100,15000)==5);
  assert(VolumeRamp::progress_percent(100,285000)==100);
  assert(VolumeRamp::progress_percent(60,165000)==100);
  assert(std::fabs(VolumeRamp::digital_gain(100,0)-0.0175f)<0.00001f);
  assert(std::fabs(VolumeRamp::digital_gain(100,15000)-0.035f)<0.00001f);
  assert(std::fabs(VolumeRamp::digital_gain(60,165000)-0.21f)<0.00001f);
  assert(std::fabs(VolumeRamp::digital_gain(100,285000)-0.35f)<0.00001f);
  return 0;
}
