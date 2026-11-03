#pragma once
#include <algorithm>
#include <cstdint>

namespace esphome::alarmclock {
// Audible alarm gain in fixed, predictable steps: 5% at the beginning,
// +5 percentage points every 15 seconds, capped at the individual alarm's
// configured target volume. This is a digital software percentage, not dB SPL.
struct VolumeRamp {
  static constexpr uint32_t STEP_MS = 15000u;
  static constexpr uint32_t UPDATE_INTERVAL_MS = STEP_MS;
  static constexpr int START_PERCENT = 5;
  static constexpr int STEP_PERCENT = 5;
  static constexpr float MAX_DIGITAL_GAIN = 0.35f;
  static constexpr uint32_t DURATION_MS = 285000u; // 5% -> 100% in 19 steps

  static int target_clamped(int target_percent) {
    return std::max(10, std::min(100, target_percent));
  }
  static uint32_t steps_to_target(int target_percent) {
    const int delta = target_clamped(target_percent) - START_PERCENT;
    return static_cast<uint32_t>((delta + STEP_PERCENT - 1) / STEP_PERCENT);
  }
  static uint32_t duration_ms(int target_percent) {
    return steps_to_target(target_percent) * STEP_MS;
  }
  static int current_percent(int target_percent, uint32_t elapsed_ms) {
    const uint32_t steps = std::min(elapsed_ms / STEP_MS, steps_to_target(target_percent));
    const int current = START_PERCENT + static_cast<int>(steps) * STEP_PERCENT;
    return std::min(target_clamped(target_percent), current);
  }
  static int progress_percent(int target_percent, uint32_t elapsed_ms) {
    const int target = target_clamped(target_percent);
    const int gained = current_percent(target, elapsed_ms) - START_PERCENT;
    const int total = target - START_PERCENT;
    return std::min(100, (gained * 100 + total / 2) / total);
  }
  static int seconds_to_next(int target_percent, uint32_t elapsed_ms) {
    if (current_percent(target_percent, elapsed_ms) >= target_clamped(target_percent)) return 0;
    const uint32_t remaining = STEP_MS - (elapsed_ms % STEP_MS);
    return static_cast<int>((remaining + 999u) / 1000u);
  }
  static int seconds_remaining(int target_percent, uint32_t elapsed_ms) {
    const uint32_t finish = duration_ms(target_percent);
    if (elapsed_ms >= finish) return 0;
    return static_cast<int>((finish - elapsed_ms + 999u) / 1000u);
  }
  static float digital_gain(int target_percent, uint32_t elapsed_ms) {
    return MAX_DIGITAL_GAIN * static_cast<float>(current_percent(target_percent, elapsed_ms)) / 100.0f;
  }
};
} // namespace esphome::alarmclock
