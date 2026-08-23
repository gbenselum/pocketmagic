# CARD-CORE-102: Plugin Manager & Swipe Gesture Navigation Framework

- **Epic:** Epic 1: Core Platform & Plugin Infrastructure
- **Priority:** High
- **Assigned Role:** `Agent-Platform`
- **Skill:** `platform-core`
- **Feature Branch:** `feature/core-102-plugin-manager`
- **Dependencies:** `CARD-CORE-101`
- **Token Budget:** ~16,000 Tokens

## 1. Description
Implement the unified `PluginManager` that registers and orchestrates `IMotionPlugin` and `IScreenPlugin` instances. Create the swipe gesture detector that listens to touch events from M5Unified/LVGL and cycles the active screen between Mode 1 (Tap Tempo), Mode 2 (Motion Gain), and Mode 3 (Config Menu).

## 2. Technical Requirements
1. Implement `PluginManager` class in `src/plugins/PluginManager.cpp`.
2. Support `registerMotionPlugin(IMotionPlugin*)` and `registerScreenPlugin(IScreenPlugin*)`.
3. Detect horizontal swipe gestures:
   - $|\Delta X| > 60\text{px}$, $|\Delta Y| < 35\text{px}$, $\Delta t < 300\text{ms}$.
   - Swipe Left: Next screen index.
   - Swipe Right: Previous screen index.
4. Clean lifecycle transitions: call `destroyUI()` on current screen, `buildUI()` on next screen.

## 3. Acceptance Criteria
- [ ] Registered screen plugins cycle predictably upon swipe gestures.
- [ ] No memory leaks during screen transitions.
- [ ] Active motion plugin feeds processed data to active screen UI safely.
