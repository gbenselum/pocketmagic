# CARD-IMU-301: MPU6886 Driver & Pitch Motion Engine

- **Epic:** Epic 3: Motion Sensor & IMU Mapping Plugins
- **Priority:** High
- **Assigned Role:** `Agent-IMU`
- **Skill:** `imu-dsp`
- **Feature Branch:** `feature/imu-301-mpu6886-engine`
- **Dependencies:** `CARD-CORE-102`
- **Token Budget:** ~25,000 Tokens

## 1. Description
Implement the 100Hz IMU sampling engine for the M5Stack Core2's integrated MPU6886 sensor using `M5Unified`. Compute accurate Pitch and Roll angles with Complementary and Exponential Moving Average (EMA) filtering, supporting resting baseline calibration.

## 2. Technical Requirements
1. Run periodic 100Hz loop inside `Task_IMU` on Core 1 using `vTaskDelayUntil()`.
2. Extract accelerometer ($A_x, A_y, A_z$) and gyroscope ($G_x, G_y, G_z$) data via `M5.Imu.getAccelData()` and `M5.Imu.getGyroData()`.
3. Compute pitch angle using $\text{Pitch} = \text{atan2}(a_x, \sqrt{a_y^2 + a_z^2}) \cdot \frac{180}{\pi}$.
4. Fuse with gyro rate using Complementary Filter ($\alpha = 0.96$).
5. Implement zero calibration function `calibrateBaseline()` to store resting neck orientation offset.

## 3. Acceptance Criteria
- [ ] IMU loop runs steadily at 100Hz ($\pm 2\text{ms}$) without starving other tasks.
- [ ] Pitch angle remains stable without drift over extended periods.
- [ ] Baseline calibration resets neutral angle to $0^\circ$.
