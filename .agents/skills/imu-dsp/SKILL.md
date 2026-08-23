---
name: imu-dsp
description: >-
  Runbook for Agent-IMU implementing MPU6886 sensor polling, Complementary/EMA pitch filtering, baseline calibration, and PitchGain motion plugin.
---

# Skill: IMU Motion DSP & Motion Plugins

This skill guides **`Agent-IMU`** in executing Epic 3 cards (`CARD-IMU-301`, `CARD-IMU-302`).

## Key DSP Algorithms
1. **Sampling & Rates:**
   - 100Hz periodic execution (`vTaskDelayUntil` with 10ms period) on Core 1.
   - Read 3-axis accelerometer and 3-axis gyroscope via `M5.Imu.getAccelData()` and `M5.Imu.getGyroData()`.
2. **Pitch Calculation & Complementary Filter:**
   - $\text{Pitch}_{\text{accel}} = \text{atan2}(a_x, \sqrt{a_y^2 + a_z^2}) \cdot \frac{180}{\pi}$
   - $\text{Pitch} = \alpha \cdot (\text{Pitch} + \text{gyro}_y \cdot \Delta t) + (1 - \alpha) \cdot \text{Pitch}_{\text{accel}}$ (typically $\alpha = 0.96$).
3. **EMA Smoothing & Deadband:**
   - Apply Exponential Moving Average: $y[n] = \beta \cdot x[n] + (1 - \beta) \cdot y[n-1]$.
   - Apply deadband ($\pm 1.0^\circ$) to eliminate sensor jitter when headstock is held still.
4. **PitchGain Mapping (POC Mode 2):**
   - Active neck tilt range: $+10^\circ \to +50^\circ$.
   - Linear or logarithmic mapping from tilt angle to Gain $(0 \dots 100)$.
   - Clamp target values strictly between 0 and 100.

## Verification Steps
- Validate zero pitch drift during static holding.
- Confirm smooth linear ramp response across full neck tilt range.
