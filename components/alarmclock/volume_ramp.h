#pragma once
#include <algorithm>
#include <cstdint>

namespace esphome::alarmclock {
// Smooth 60-second gain ramp. Digital gain is refreshed every second, with
// display percent rounded to whole numbers. Values are not acoustic dB.
struct VolumeRamp {
  static constexpr uint32_t DURATION_MS = 60000u;
  static constexpr uint32_t UPDATE_INTERVAL_MS = 1000u;
  static constexpr int START_FACTOR_PERCENT = 50;
  static constexpr float MAX_DIGITAL_GAIN = 0.35f;

  static uint32_t elapsed_clamped(uint32_t elapsed_ms) {
    return std::min(elapsed_ms, DURATION_MS);
  }
  static int progress_percent(uint32_t elapsed_ms) {
    return static_cast<int>((static_cast<uint64_t>(elapsed_clamped(elapsed_ms)) * 100u + DURATION_MS/2u) / DURATION_MS);
  }
  static float gain_factor(uint32_t elapsed_ms) {
    return 0.5f + 0.5f * static_cast<float>(elapsed_clamped(elapsed_ms)) / static_cast<float>(DURATION_MS);
  }
  static int current_percent(int target_percent, uint32_t elapsed_ms) {
    const int target = std::max(10, std::min(100, target_percent));
    const uint32_t elapsed = elapsed_clamped(elapsed_ms);
    // Integer arithmetic for a stable UI percentage.
    const uint32_t factor_milli = 500u + (elapsed * 500u) / DURATION_MS;
    return static_cast<int>((target * factor_milli + 500u) / 1000u);
  }
  static int seconds_to_next(uint32_t elapsed_ms) {
    if (elapsed_ms >= DURATION_MS) return 0;
    const uint32_t remaining = UPDATE_INTERVAL_MS - (elapsed_ms % UPDATE_INTERVAL_MS);
    return static_cast<int>((remaining + 999u) / 1000u);
  }
  static int seconds_remaining(uint32_t elapsed_ms) {
    if (elapsed_ms >= DURATION_MS) return 0;
    return static_cast<int>((DURATION_MS-elapsed_ms+999u)/1000u);
  }
  static float digital_gain(int target_percent, uint32_t elapsed_ms) {
    const int target = std::max(10, std::min(100, target_percent));
    return MAX_DIGITAL_GAIN * static_cast<float>(target) / 100.0f * gain_factor(elapsed_ms);
  }
};
} // namespace esphome::alarmclock
