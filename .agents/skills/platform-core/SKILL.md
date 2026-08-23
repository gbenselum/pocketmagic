---
name: platform-core
description: >-
  Runbook for Agent-Platform implementing FreeRTOS dual-core initialization, M5Unified hardware abstraction, Plugin Manager, and swipe navigation framework.
---

# Skill: Platform Core & Plugin Framework

This skill guides **`Agent-Platform`** in executing Epic 1 cards (`CARD-CORE-101`, `CARD-CORE-102`).

## Key Responsibilities
1. **Initialize M5Unified:**
   - Call `M5.begin()` to initialize PMIC (AXP192/AXP2101), Display (ILI9342C), Touch Controller (FT6336U), and IMU (MPU6886).
   - Configure display rotation (typically 1 or 3 for landscape).
2. **Spawn FreeRTOS Dual-Core Tasks:**
   - `xTaskCreatePinnedToCore(Task_BLE, "Task_BLE", 4096, NULL, 3, &hTaskBLE, 0)`
   - `xTaskCreatePinnedToCore(Task_Protocol, "Task_Protocol", 4096, NULL, 3, &hTaskProtocol, 0)`
   - `xTaskCreatePinnedToCore(Task_IMU, "Task_IMU", 4096, NULL, 4, &hTaskIMU, 1)`
   - `xTaskCreatePinnedToCore(Task_GUI, "Task_GUI", 8192, NULL, 2, &hTaskGUI, 1)`
3. **Implement Plugin Manager:**
   - Maintain active lists of `IMotionPlugin` and `IScreenPlugin`.
   - Provide clean registration methods: `registerMotionPlugin()`, `registerScreenPlugin()`.
   - Implement swipe gesture detector to switch active screen plugin (`Mode 1 <-> Mode 2 <-> Mode 3`).

## Verification Steps
- Check that FreeRTOS tasks start cleanly on assigned cores without watchdog triggers (`esp_task_wdt`).
- Ensure memory check shows > 100KB free heap and no memory leaks during screen transitions.
