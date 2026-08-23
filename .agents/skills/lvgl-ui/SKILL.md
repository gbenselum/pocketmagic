---
name: lvgl-ui
description: >-
  Runbook for Agent-UI implementing 320x240 LVGL touch screens, Mode 1 Tap Tempo, Mode 2 Live Bar Graph, and Mode 3 Config Menu.
---

# Skill: Touchscreen HCI & LVGL UI Development

This skill guides **`Agent-UI`** in executing Epic 4 cards (`CARD-UI-401`, `CARD-UI-402`).

## Screen Specifications (320x240 M5Stack Core2 Display)

### 1. Mode 1: Tap Tempo Screen (`IScreenPlugin`)
- **Target Area:** Large, prominent pulsating button centered on screen.
- **BPM Calculator:**
  - Evaluates time delta ($\Delta t_{\text{ms}}$) between consecutive taps.
  - Computes $\text{BPM} = \frac{60000}{\Delta t_{\text{ms}}}$.
  - Averaged over the last 4 taps; resets if idle for $>2500\text{ms}$.
  - Transmits Delay Time SysEx packet (`parameters[7][1][ms_value]`).
- **Visuals:** Active BPM readout, animated pulse ring, note division indicator (1/4, 1/8, dotted 1/8).

### 2. Mode 2: Motion Gain Screen (`IScreenPlugin`)
- **Visuals:** Real-time vertical/horizontal bar meter showing neck tilt angle ($+10^\circ \to +50^\circ$) and calculated Gain ($0\% \to 100\%$).
- **Dynamic Color:** Color gradient transition (Green -> Yellow -> Red) as gain increases.

### 3. Mode 3: Config & Calibration Menu (`IScreenPlugin`)
- **Controls:**
  - `Calibrate Neutral Posture` button: Zeroes IMU baseline angle.
  - `BLE Connect / Reconnect` toggle button.
  - Battery gauge (voltage + percentage from PMIC) and BLE RSSI meter.

## Touch & Gesture Discrimination
- Differentiate Tap from Swipe:
  - **Tap:** $\Delta X < 20\text{px}$, $\Delta Y < 20\text{px}$, touch duration $< 250\text{ms}$.
  - **Swipe:** $|\Delta X| > 60\text{px}$, $|\Delta Y| < 35\text{px}$, swipe duration $< 300\text{ms}$.

## Verification Steps
- Verify fluid rendering at 30+ FPS.
- Verify no touch dropouts during rapid double/triple tap tempo inputs.
