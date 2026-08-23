# CARD-UI-401: POC Mode 1: Tap Tempo Touch Screen Plugin

- **Epic:** Epic 4: Touchscreen UI & POC Screen Plugins
- **Priority:** High
- **Assigned Role:** `Agent-UI`
- **Skill:** `lvgl-ui`
- **Feature Branch:** `feature/ui-401-tap-tempo`
- **Dependencies:** `CARD-CORE-102`
- **Token Budget:** ~28,000 Tokens

## 1. Description
Implement the Mode 1 UI screen plugin using LVGL for Tap Tempo control. The user taps the touch display to set the tempo; the plugin calculates BPM, shows visual feedback, and triggers a Delay Time parameter update over BLE.

## 2. Technical Requirements
1. Implement `IScreenPlugin` interface for Mode 1.
2. Build UI layout with 320x240 resolution:
   - Large interactive central Tap button with pulsating animation ring on touch.
   - Live numeric BPM display ($40 \dots 300\text{BPM}$).
   - Note division selector (1/4, 1/8, dotted 1/8).
3. Compute inter-tap interval $\Delta t_{\text{ms}}$ using moving average of last 4 taps.
4. Post Delay Time write command (`parameters[7][1][ms_value]`) to `xBleWriteQueue` on valid tap sequences.

## 3. Acceptance Criteria
- [ ] Responsive touch target evaluates tap intervals accurately within $\pm 1\text{BPM}$.
- [ ] Resets tap sequence if idle for $>2.5\text{seconds}$.
- [ ] Does not trigger false swipe transitions during rapid tapping.
