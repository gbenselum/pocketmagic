# FreeRTOS Concurrency & Dual-Core Architecture Rules

## 1. Dual-Core Affinity
* **Core 0 (Comms Core):**
  - `Task_BLE`: NimBLE Central GATT connection management, auto-reconnect, notification listener. (Priority: 3, Stack: 4096 bytes).
  - `Task_Protocol`: SysEx packet encoder, rate limiter (max 20 writes/sec), GATT write executor. (Priority: 3, Stack: 4096 bytes).
* **Core 1 (Application / UI Core):**
  - `Task_IMU`: 100Hz I2C sampling of MPU6886, EMA/Complementary filter, motion plugin processor. (Priority: 4, Stack: 4096 bytes).
  - `Task_GUI`: LVGL timer handler (`lv_timer_handler()`), touch input polling, swipe detector, screen rendering. (Priority: 2, Stack: 8192 bytes).

## 2. Inter-Task Communication
* **Queue-Driven Commands:**
  - Tasks communicate strictly via FreeRTOS Queues (`xMotionQueue`, `xBleWriteQueue`).
  - Never share mutable state or pointers across Core 0 and Core 1 without a mutex or queue copy.
* **Non-Blocking Queue Posting:**
  - When `Task_IMU` produces a parameter update, it posts to `xBleWriteQueue` with a timeout of `0` or `pdMS_TO_TICKS(5)`. If the queue is full, drop the stale motion frame rather than blocking the 100Hz sensor loop.

## 3. Rate Limiting & Flow Control
* The BLE communication task enforces a **leaky bucket / token rate limiter** bounded to **20 writes per second** (50ms interval).
* Identical consecutive parameter updates must be filtered using a deadband threshold to prevent unnecessary Bluetooth traffic.
