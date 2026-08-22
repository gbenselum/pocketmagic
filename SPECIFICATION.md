# SPECIFICATION: ESP32 Headstock Motion Controller for Sonicake Pocket Master

## 1. Executive Summary & Project Goal

This specification defines the spec-driven design for an out-of-the-box, no-solder **ESP32-based Motion & Touchscreen Controller** designed to be mounted on the headstock of a guitar or bass.

The controller communicates wirelessly over **Bluetooth Low Energy (BLE)** (or optionally USB MIDI) with the **Sonicake Pocket Master** multi-effects pedal. Utilizing a 6-axis Inertial Measurement Unit (IMU - 3-axis accelerometer + 3-axis gyroscope), the performer can control effect parameters dynamically through physical instrument motion (e.g., tilting the neck up/down for Gain or Wah, pitching/rolling for Chorus speed or Delay feedback) and interact with a touch-driven graphical UI for preset management, parameter mapping, and Tap Tempo.

This design is structured to serve as the blueprint for downstream software engineering agents to implement the firmware autonomously, leveraging a **modular plugin architecture** and **Jira-style task breakdown measured in Tokens** for parallel multi-agent development and context budgeting.

---

## 2. Hardware Architecture & Hardware Recommendations

### 2.1 Hardware Requirements
To satisfy the strict constraint of **zero soldering**, the device must be a fully integrated, commercial off-the-shelf ESP32 development enclosure containing:
1. **ESP32 Microcontroller** (ESP32-S3 or ESP32-D0WDQ6) with BLE 4.2 / 5.0 support.
2. **Capacitive Touchscreen** (color TFT or LCD, minimum 1.1" - 2.0").
3. **6-Axis IMU** (integrated Accelerometer + Gyroscope, e.g., MPU6886, BMI270, or LSM6DS3).
4. **Internal Rechargeable LiPo Battery** & Power Management IC (PMIC).
5. **Enclosure / Shell** suitable for mounting via a clip, rubber strap, or 1/4" mount to a guitar headstock.

### 2.2 Comparison of Off-the-Shelf Candidate Devices

| Feature | Option A: M5Stack Core2 v1.1 / Core2 | Option B: M5StickC PLUS2 + Battery Base | Option C: LilyGO T-Display-S3 Touch + IMU | Option D: M5Stack M5Tough |
| :--- | :--- | :--- | :--- | :--- |
| **Display** | 2.0" Capacitive Touch TFT (320x240) | 1.14" IPS LCD (135x240) + Buttons (No Touch) | 1.9" Capacitive Touch TFT (170x320) | 2.0" Touch TFT (320x240) Waterproof Enclosure |
| **6-Axis IMU** | MPU6886 (integrated) | MPU6886 (integrated) | ST7789 + Ax/Gyro (Optional module) | MPU6886 (integrated) |
| **Battery** | 500mAh (with M5GO Bottom2) or 390mAh base | 200mAh (integrated) | 470mAh JST / Shell | 500mAh (integrated) |
| **Dimensions** | 54mm x 54mm x 16mm | 48mm x 25mm x 13mm | 62mm x 26mm x 15mm | 54mm x 54mm x 22mm |
| **Weight** | ~52g (lightweight) | ~15g (ultra-light) | ~35g | ~70g |
| **Headstock Mounting** | Rubber clip / 3D clip / Magnet | Guitar tuner clip style | Clip / Strap | Heavy-duty clip |
| **Soldering Needed** | **NONE** | **NONE** | Minimal / Shell assembly | **NONE** |
| **Recommendation** | **PRIMARY RECOMMENDATION (10/10)** | Secondary (No touch) | Alternate Choice | Rugged Alternate |

### 2.3 Selected Hardware Platform: M5Stack Core2 v1.1
* **Processor:** ESP32-D0WDQ6-V3 (Dual Core LX6 @ 240MHz), 16MB Flash, 8MB PSRAM.
* **Display:** 2.0" Capacitive Touch Screen (320x240 ILI9342C).
* **Motion Sensor:** MPU6886 6-axis IMU (3-axis accelerometer + 3-axis gyroscope) via I2C (`0x68`).
* **Power & Battery:** AXP2101 PMIC + 500mAh integrated LiPo battery.
* **Mounting Solution:** Standard guitar tuner headstock clamp attached to the LEGO/M3 screw holes on the M5GO Bottom2 base.

---

## 3. Sonicake Pocket Master BLE & SysEx Protocol Specification

The device operates as a **BLE Central** (Master) that scans for and connects to the Sonicake Pocket Master pedal (**BLE Peripheral**).

### 3.1 BLE Connection Details
* **Device Name Filter:** `Sonic Master BLE`
* **GATT Service UUID:** `03b80e5a-ede8-4b33-a751-6ce34ec4c700`
* **GATT Characteristic UUID:** `7772e5db-3868-4112-a1a9-f2669d106bf3` (Read, Write Without Response, Notify)

### 3.2 Protocol Packet Framing
Commands sent to the pedal use MIDI SysEx framing wrapped in a 2-byte prefix:
`[80 80] [F0] [CRC_HIGH] [CRC_LOW] [PAYLOAD] [F7]`

* **Header:** `80 80 F0`
* **CRC-8 SMBus PEC Checksum (2 nibbles expanded):** Calculated on the raw (unexpanded) payload bytes.
* **Payload:** Nibble-expanded hex representation of the command.
* **Terminator:** `F7`

### 3.3 CRC-8 (SMBus PEC) Calculation Algorithm
The CRC algorithm uses polynomial $x^8 + x^2 + x^1 + 1$ (`0x07`) with an initial seed of `0x00`:

```c
uint8_t calculate_smbus_crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}
```

When constructing the packet:
1. Format the raw payload bytes (e.g. `00 01 00 00 01 03 ...`).
2. Calculate `crc8` over the raw payload.
3. Nibble-expand the CRC byte into 2 bytes: e.g., CRC `0xC5` -> `0x0C 0x05`.
4. Wrap with `80 80 F0` + `[CRC_HIGH, CRC_LOW]` + `[EXPANDED_PAYLOAD]` + `F7`.

### 3.4 Module Identification
The Pocket Master multi-effects signal chain consists of 10 modules:
* `0`: Noise Reduction (**NR**)
* `1`: FX1 (Compressors, Touch Wah, Boost, Chorus, Flanger, Phaser, Tremolo)
* `2`: Overdrive / Distortion (**DRV**)
* `3`: Amplifier Simulator (**AMP** - Factory)
* `4`: Cabinet / Impulse Response (**IR**)
* `5`: Equalizer (**EQ**)
* `6`: FX2 (Modulation / Pitch / Detune)
* `7`: Delay (**DLY**)
* `8`: Reverb (**RVB**)
* `9`: Amp Profile (**Clone**)

---

## 4. Plugin System Architecture

To allow multiple coding agents to work independently on different features without breaking the core system, the firmware is organized around a **Plugin Architecture**.

```
                           +------------------------+
                           |  Plugin Manager Core   |
                           +-----------+------------+
                                       |
          +----------------------------+----------------------------+
          |                            |                            |
          v                            v                            v
+------------------+         +-------------------+        +--------------------+
|  Motion Plugins  |         |  UI Screen        |        |  Comms Backend     |
|  (IMU Processors)|         |  Plugins (LVGL)   |        |  Plugins           |
+------------------+         +-------------------+        +--------------------+
| - PitchGain      |         | - PerformView     |        | - BLE Central      |
| - ModRollChorus  |         | - MotionMapEditor |        | - USB MIDI         |
| - SpatialReverb  |         | - CalibrationUI   |        | - Virtual Simulator|
| - SnapTapTempo   |         +-------------------+        +--------------------+
+------------------+
```

### 4.1 Motion Plugin Interface (`IMotionPlugin`)
Each motion mapping algorithm implements a uniform C++ interface:

```cpp
class IMotionPlugin {
public:
    virtual ~IMotionPlugin() = default;
    virtual const char* getName() const = 0;
    virtual void init() = 0;
    virtual void processIMU(float pitch, uint16_t roll, const float accel[3], const float gyro[3]) = 0;
    virtual bool getTargetCommand(uint8_t& outModuleId, uint8_t& outAlgId, float& outValue) = 0;
    virtual void reset() = 0;
};
```

### 4.2 UI Screen Plugin Interface (`IScreenPlugin`)
Each display view operates as an isolated screen component:

```cpp
class IScreenPlugin {
public:
    virtual ~IScreenPlugin() = default;
    virtual const char* getTitle() const = 0;
    virtual void buildUI(lv_obj_t* parentScreen) = 0;
    virtual void updateUI() = 0;
    virtual void destroyUI() = 0;
};
```

---

## 5. Security & Safety Specification

### 5.1 Threat Model & Security Mitigations
1. **Unintended BLE Connection / Hijacking:**
   * *Mitigation:* BLE scanning strictly filters by device name `Sonic Master BLE` and service UUID `03b80e5a-ede8-4b33-a751-6ce34ec4c700`. Option to enable Passkey/Numeric Comparison pairing (BLE Security Level 3) if supported by hardware.
2. **Buffer Overflow & Malformed SysEx Vulnerabilities:**
   * *Mitigation:* All received BLE/SysEx packets are length-checked before parsing. Maximum packet payload length is bounded to 256 bytes. Bounds checking is enforced on all array offsets.
3. **Parameter Flood & Device Freezing:**
   * *Mitigation:* The Protocol Task enforces a **rate limiter (bounded to max 20 BLE write tokens per second)** for motion-triggered parameter updates. Motion delta thresholding prevents spamming identical byte writes.
4. **Out-of-Bounds Motion Values:**
   * *Mitigation:* All motion sensor readings pass through clamping functions (`std::clamp(val, min, max)`) before SysEx byte encoding to prevent parameter corruption.

---

## 6. Open Source Licensing & Third-Party Compatibility

### 6.1 Project License
This project and all associated code/firmware developed under this design are licensed under the **MIT License**.

```text
MIT License

Copyright (c) 2026

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

### 6.2 Third-Party Library License Compatibility Matrix

| Library / Dependency | License | Permitted for MIT Redistribution? | Compliance Strategy |
| :--- | :--- | :--- | :--- |
| **ESP-IDF / ESP32 Arduino Core** | Apache 2.0 | **Yes** | Apache 2.0 is compatible with MIT. |
| **M5Unified / M5Core2** | MIT | **Yes** | Native MIT compatibility. |
| **LVGL (Light and Versatile Graphics Library)** | MIT | **Yes** | Native MIT compatibility. |
| **NimBLE-Arduino / ESP32 BLE** | Apache 2.0 / MIT | **Yes** | Apache 2.0/MIT compatible. |
| **FreeRTOS** | MIT (with Exception) | **Yes** | Standard FreeRTOS kernel license. |

---

## 7. Emulation & Virtual Simulation Strategy

To enable autonomous software engineering agents and human developers to build, test, and verify the ESP32 firmware without requiring physical hardware immediately on hand, three simulation layers are specified:

### 7.1 Wokwi ESP32 Hardware Simulator (Firmware & IMU Emulation)
* **Platform:** [Wokwi.com](https://wokwi.com/)
* **Emulated Components:**
  * ESP32-S3 / ESP32 Dual Core board model.
  * ILI9341 / ST7789 TFT display + touch screen module (`diagram.json`).
  * MPU6050 / MPU6886 I2C IMU sensor module.
* **IMU Motion Simulation:**
  * Wokwi provides interactive GUI sliders for Accel X/Y/Z and Gyro X/Y/Z to simulate tilting the headstock up/down (Pitch) and twisting (Roll) in real time.
* **CLI Execution:** Supports `wokwi-cli` in continuous integration (CI) workflows to execute automated C++/Arduino/ESP-IDF tests.

### 7.2 LVGL Desktop Simulator (Touchscreen HCI Prototyping)
* **Platform:** LVGL Desktop Simulator (VS Code / SDL2 for Linux/macOS/Windows) or LVGL WebAssembly.
* **Target Screen Resolution:** 320x240 pixels (matching M5Stack Core2).
* **Usage:** Allows rapid UI development, widget layout verification, mouse-driven touch simulation, and Tap Tempo button responsiveness testing before flashing onto ESP32 hardware.

### 7.3 Virtual Pocket Master BLE Peripheral Emulator (End-to-End Comms)
To test BLE GATT client discovery and SysEx command validation without the physical pedal:
* **Option A (Python BLE Emulator):** A lightweight Python script using `bleak` / `bleno` running on a PC/laptop that advertises as `Sonic Master BLE` with GATT Characteristic `7772e5db-3868-4112-a1a9-f2669d106bf3`.
* **Option B (Local PocketEdit Web App):** Running `index.html` from `PocketEdit` connected via Web Bluetooth or Virtual USB MIDI to visually confirm that transmitted commands correctly manipulate the virtual pedal controls.

---

## 8. Multi-Agent Development Backlog & Token Metrics (Jira-Style Cards)

Tasks are structured into **5 Epics** with independent, parallelizable **Task Cards**. All workload estimations, agent allocation budgets, and execution complexity metrics are expressed in **Token Budgets** (measured as LLM prompt & completion context window consumption per agent implementation turn):

### Token Measurement Scale
* **XS (Small Task):** ~5,000 – 12,000 Tokens (Single file edit / driver stub / isolated helper)
* **S (Medium Task):** ~12,000 – 25,000 Tokens (Feature module / protocol encoder / screen view)
* **M (Large Task):** ~25,000 – 45,000 Tokens (System engine / multi-file plugin / driver integration)
* **L (Complex Epic/Integration):** ~45,000 – 80,000 Tokens (End-to-end integration / full UI stack / emulator suite)

```
================================================================================
EPIC 1: CORE PLATFORM & PLUGIN INFRASTRUCTURE
================================================================================

[CARD-CORE-101] Project Bootstrap & FreeRTOS Dual-Core Task Skeleton
- Priority: High | Component: Core Firmware | Dependencies: None
- Assigned Agent Role: Agent-Platform
- Token Budget: ~18,000 Tokens (Max Context Window Target)
- Description: Create the base CMake/PlatformIO ESP32 project structure with MIT License header.
  Setup FreeRTOS tasks distributed across Core 0 (Comms) and Core 1 (UI/IMU).
- Acceptance Criteria:
  1. Compiles with ESP-IDF / Arduino Framework under MIT License.
  2. Spawns Task_BLE and Task_Protocol on Core 0, Task_IMU and Task_GUI on Core 1.
  3. Memory check shows zero memory leaks or stack overflows.

[CARD-CORE-102] Plugin Manager & Interface Definitions
- Priority: High | Component: Architecture | Dependencies: CARD-CORE-101
- Token Budget: ~15,000 Tokens
- Assigned Agent Role: Agent-Platform
- Description: Implement IMotionPlugin, IScreenPlugin, and ICommsBackend abstract C++ interfaces
  along with a dynamic PluginManager registry class.
- Acceptance Criteria:
  1. Plugins can register and unregister dynamically at runtime.
  2. PluginManager routes IMU data ticks to active motion plugins.
  3. Clean separation of header files in `src/plugins/`.

================================================================================
EPIC 2: COMMUNICATION & PROTOCOL PLUGINS
================================================================================

[CARD-BLE-201] Sonicake BLE Central Client Plugin
- Priority: High | Component: Comms Backend | Dependencies: CARD-CORE-102
- Token Budget: ~30,000 Tokens
- Assigned Agent Role: Agent-Comms
- Description: Build NimBLE BLE Central client that scans for "Sonic Master BLE", connects,
  discovers Service `03b80e5a...` and Characteristic `7772e5db...`, and subscribes to notifications.
- Acceptance Criteria:
  1. Auto-connects and auto-reconnects on connection loss.
  2. Thread-safe write queue consumed by Core 0 BLE task.
  3. Bounded rate limiter (max 20 GATT write tokens / sec).

[CARD-BLE-202] SysEx Packet Encoder & CRC-8 SMBus Checksum
- Priority: High | Component: Protocol | Dependencies: CARD-CORE-102
- Token Budget: ~22,000 Tokens
- Assigned Agent Role: Agent-Comms
- Description: Implement SysEx packet formatter wrapping `80 80 F0 [CRC8_EXP] [PAYLOAD] F7`
  and CRC-8 SMBus PEC calculation (`0x07` polynomial).
- Acceptance Criteria:
  1. CRC-8 calculation matches `PocketEdit` reference output for all test vectors.
  2. Bounds checking prevents payload overflow (> 256 bytes rejected).
  3. Unit tests pass for preset switch, volume change, and parameter writes.

================================================================================
EPIC 3: MOTION SENSOR & IMU MAPPING PLUGINS
================================================================================

[CARD-IMU-301] MPU6886 Driver & Complementary Motion Engine
- Priority: High | Component: Sensor Plugin | Dependencies: CARD-CORE-102
- Token Budget: ~25,000 Tokens
- Assigned Agent Role: Agent-IMU
- Description: Implement MPU6886 I2C reader running at 100 Hz, applying Exponential Moving
  Average (EMA) filtering and Complementary Filter for Pitch and Roll angles.
- Acceptance Criteria:
  1. Outputs stable Pitch and Roll angles (-90° to +90°) with zero drift.
  2. Calibrate rest position functionality implemented.
  3. Zero I2C bus blocking on Task_IMU execution.

[CARD-IMU-302] Motion Mapping Plugins (Pitch-Gain, Mod-Roll, Spatial-Reverb)
- Priority: Medium | Component: Motion Plugins | Dependencies: CARD-IMU-301
- Token Budget: ~35,000 Tokens
- Assigned Agent Role: Agent-IMU
- Description: Create three `IMotionPlugin` modules:
  1. `PitchGainPlugin`: Maps neck pitch angle to DRV/AMP Gain.
  2. `ModRollChorusPlugin`: Maps headstock roll angle to Chorus Rate/Depth.
  3. `SnapTapTempoPlugin`: Detects >2.2g acceleration snap gesture to calculate Tap Tempo.
- Acceptance Criteria:
  1. Smooth parameter interpolation with deadband thresholding.
  2. Snap gesture reliably measures inter-tap interval for BPM calculation.

================================================================================
EPIC 4: TOUCHSCREEN UI & HCI PLUGINS
================================================================================

[CARD-UI-401] LVGL Performance Dashboard Screen Plugin
- Priority: High | Component: Touch UI | Dependencies: CARD-CORE-102
- Token Budget: ~38,000 Tokens
- Assigned Agent Role: Agent-UI
- Description: Implement main Performance View screen using LVGL (320x240 resolution):
  - Displays current Preset Name & Number.
  - Live Pitch/Roll Motion Bar Graph.
  - Large Touch TAP TEMPO button with visual beat pulse.
  - Patch increment/decrement buttons.
- Acceptance Criteria:
  1. Smooth rendering without screen tearing.
  2. Touch tap tempo button calculates BPM accurately.
  3. Preset selection updates display and triggers BLE command queue.

[CARD-UI-402] Motion Mapper & Calibration Screen Plugins
- Priority: Medium | Component: Touch UI | Dependencies: CARD-UI-401
- Token Budget: ~28,000 Tokens
- Assigned Agent Role: Agent-UI
- Description: Build secondary UI screens for selecting active motion plugin, adjusting
  sensitivity/deadband sliders, and zeroing IMU resting position.
- Acceptance Criteria:
  1. Swipe or tab gesture switches screens cleanly.
  2. Calibration button zeros resting pitch/roll baseline immediately.

================================================================================
EPIC 5: SIMULATION & TESTING PLUGINS
================================================================================

[CARD-SIM-501] Wokwi Simulator Configuration & Python Virtual BLE Peripheral
- Priority: Medium | Component: Testing | Dependencies: CARD-BLE-201, CARD-BLE-202
- Token Budget: ~20,000 Tokens
- Assigned Agent Role: Agent-QA
- Description: Provide `diagram.json` for Wokwi ESP32-S3 + MPU6050 + ILI9341 touch simulation,
  and a standalone Python script `virtual_pocket_master.py` that emulates the BLE pedal.
- Acceptance Criteria:
  1. Python script advertises as "Sonic Master BLE" and logs incoming SysEx packets.
  2. Wokwi interactive sliders allow simulated tilt motion testing in browser.
```

---

## 9. Summary & License Confirmation
This specification delivers a modular, **MIT-licensed**, security-hardened, and **plugin-based architecture** for an off-the-shelf ESP32 headstock motion controller. All task estimations, context window limits, and agent execution metrics are measured in **Tokens**, allowing autonomous development agents to claim, budget, and implement platform, comms, IMU, UI, and simulation modules in parallel.
