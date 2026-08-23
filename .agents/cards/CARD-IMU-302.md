# CARD-IMU-302: PitchGain Motion Plugin (POC Mode 2)

- **Epic:** Epic 3: Motion Sensor & IMU Mapping Plugins
- **Priority:** Medium
- **Assigned Role:** `Agent-IMU`
- **Skill:** `imu-dsp`
- **Feature Branch:** `feature/imu-302-pitchgain-plugin`
- **Dependencies:** `CARD-IMU-301`
- **Token Budget:** ~20,000 Tokens

## 1. Description
Implement the `PitchGainPlugin` implementing `IMotionPlugin`. Maps guitar neck tilt (+10° to +50°) to DRV (Overdrive) or AMP (Amplifier) Gain (0 to 100), including deadband filtering and thresholding.

## 2. Technical Requirements
1. Implement `IMotionPlugin` methods: `getName()`, `init()`, `processIMU()`, `getTargetCommand()`, `reset()`.
2. Map pitch tilt from active range $[+10^\circ, +50^\circ]$ to $[0, 100]$.
3. Apply deadband thresholding ($\Delta \text{Gain} \ge 1$) to avoid redundant packet transmission when held steady.
4. Output commands targeting DRV Module (ID: 2, Param: 0) or AMP Module (ID: 3, Param: 0).

## 3. Acceptance Criteria
- [ ] Smooth interpolation from 0% Gain at $+10^\circ$ tilt to 100% Gain at $+50^\circ$ tilt.
- [ ] No SysEx commands dispatched when pitch changes are within deadband limit.
- [ ] Values clamped strictly between 0 and 100.
