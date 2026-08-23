# CARD-UI-402: POC Mode 2: Motion Gain & Mode 3 Config Screen Plugins

- **Epic:** Epic 4: Touchscreen UI & POC Screen Plugins
- **Priority:** High
- **Assigned Role:** `Agent-UI`
- **Skill:** `lvgl-ui`
- **Feature Branch:** `feature/ui-402-gain-and-config-screens`
- **Dependencies:** `CARD-UI-401`
- **Token Budget:** ~32,000 Tokens

## 1. Description
Implement Mode 2 (Live Neck Tilt Gain Bar Graph) and Mode 3 (Config & Calibration Menu) screen plugins conforming to `IScreenPlugin`.

## 2. Technical Requirements
1. **Mode 2 Screen:**
   - Real-time animated bar meter showing pitch angle ($+10^\circ \dots +50^\circ$) and calculated Gain level ($0\% \dots 100\%$).
   - Color gradient styling (Green for low gain, Yellow for medium, Red for lead boost).
2. **Mode 3 Screen:**
   - Button: `Calibrate Resting Posture` (invokes zero calibration on IMU).
   - Button: `BLE Reconnect / Pair` toggle.
   - Status indicators: Battery level / voltage and BLE connection state.
3. Hook into `PluginManager` swipe transitions.

## 3. Acceptance Criteria
- [ ] Mode 2 updates bar graph smoothly at >= 30 FPS based on live IMU pitch data.
- [ ] Mode 3 calibration button zeroes resting angle correctly.
- [ ] Memory footprint remains stable after 50+ back-and-forth screen swipes.
