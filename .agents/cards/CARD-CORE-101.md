# CARD-CORE-101: Project Bootstrap & FreeRTOS Dual-Core Task Skeleton

- **Epic:** Epic 1: Core Platform & Plugin Infrastructure
- **Priority:** High
- **Assigned Role:** `Agent-Platform`
- **Skill:** `platform-core`
- **Feature Branch:** `feature/core-101-freertos-skeleton`
- **Dependencies:** None
- **Token Budget:** ~18,000 Tokens

## 1. Description
Bootstrap the PlatformIO ESP32 project structure with `M5Unified`, MIT License headers, and establish the FreeRTOS dual-core task layout:
* **Core 0 (Comms Core):** `Task_BLE` (Stack 4KB, Priority 3) and `Task_Protocol` (Stack 4KB, Priority 3).
* **Core 1 (Application Core):** `Task_IMU` (Stack 4KB, Priority 4) and `Task_GUI` (Stack 8KB, Priority 2).
* Setup inter-task FreeRTOS queues: `xBleWriteQueue` and `xMotionEventQueue`.

## 2. Technical Requirements
1. Initialize M5Unified (`M5.begin()`) in `src/main.cpp`.
2. Configure task pinning using `xTaskCreatePinnedToCore()`.
3. Implement heartbeat logging for each task under debug level.
4. Ensure zero memory leaks or watchdog timer resets.

## 3. Acceptance Criteria
- [ ] Firmware compiles cleanly under `m5stack-core2` environment.
- [ ] Serial monitor displays successful initialization of M5Unified, Core 0 tasks, and Core 1 tasks.
- [ ] Stack high-water mark checks show sufficient headroom (> 1KB per task).
- [ ] Code passes embedded C++ guidelines.
