# Embedded C++ Guidelines for ESP32

## 1. C++ Standard & Compiler
* Target standard: **C++17** (or **C++20** when supported by toolchain).
* Avoid exceptions (`-fno-exceptions`) and RTTI (`-fno-rtti`) to minimize flash binary footprint and deterministic execution time.

## 2. Memory & Allocation
* **Zero Dynamic Allocation in Real-Time Loops:**
  Do NOT use `new`, `malloc`, `std::vector::push_back` (without pre-reserve), or `std::string` concatenation in `Task_IMU` (100Hz) or `Task_GUI` (30-60Hz).
* Use static pre-allocated arrays, `std::array`, or fixed-size ring buffers.
* Stack allocations inside FreeRTOS tasks must remain well below task stack limits (typical stack is 4KB - 8KB).

## 3. Defensive Programming & Safety
* Always clamp input values before mathematical operations or byte conversion using `std::clamp()`.
* Validate array indices and payload lengths against bounds (`MAX_SYSEX_PAYLOAD_SIZE = 256`).
* Use `constexpr` and `const` wherever values are immutable.
* Wrap hardware access in defensive checks (e.g. check I2C return codes or M5 initialization state).

## 4. Hardware Driver Abstraction
* Use `M5Unified` APIs (`M5.begin()`, `M5.Imu`, `M5.Display`, `M5.Power`, `M5.Touch`) to ensure code works on M5Stack Core2, Core2 v1.1, and Tough.
* Do not access raw ESP32 registers or legacy `M5Core2` header files directly.
