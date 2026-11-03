#pragma once
#include <algorithm>
#include <cstdint>

namespace esphome::alarmclock {
// Ten deliberate volume steps spread evenly over one minute.
// Percent values are UI percent of the user's selected maximum, not dB/SPL.
struct VolumeRamp {
  static constexpr uint32_t DURATION_MS = 60000u;
  static constexpr uint32_t STEP_MS = 6000u;
  static constexpr int START_FACTOR_PERCENT = 50;
  static constexpr int STEP_FACTOR_PERCENT = 5;
  static constexpr float MAX_DIGITAL_GAIN = 0.35f; // bounded to protect small speaker

  static int step(uint32_t elapsed_ms) {
    return static_cast<int>(std::min<uint32_t>(10u, elapsed_ms / STEP_MS));
  }
  static int current_percent(int target_percent, uint32_t elapsed_ms) {
    const int target = std::max(10, std::min(100, target_percent));
    const int factor = START_FACTOR_PERCENT + STEP_FACTOR_PERCENT * step(elapsed_ms);
    return (target * factor + 50) / 100;
  }
  static int seconds_to_next(uint32_t elapsed_ms) {
    if (elapsed_ms >= DURATION_MS) return 0;
    const uint32_t remaining = STEP_MS - (elapsed_ms % STEP_MS);
    return static_cast<int>((remaining + 999u) / 1000u);
  }
  static float digital_gain(int target_percent, uint32_t elapsed_ms) {
    return MAX_DIGITAL_GAIN * static_cast<float>(current_percent(target_percent, elapsed_ms)) / 100.0f;
  }
};
} // namespace esphome::alarmclock
