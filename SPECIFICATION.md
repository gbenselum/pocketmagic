# SPECIFICATION: ESP32 Headstock Motion Controller for Sonicake Pocket Master

## 1. Executive Summary & Project Goal

This specification defines the spec-driven design for an out-of-the-box, no-solder **ESP32-based Motion & Touchscreen Controller** designed to be mounted on the headstock of a guitar or bass.

The controller communicates wirelessly over **Bluetooth Low Energy (BLE)** (or optionally USB MIDI) with the **Sonicake Pocket Master** multi-effects pedal. Utilizing a 6-axis Inertial Measurement Unit (IMU - 3-axis accelerometer + 3-axis gyroscope), the performer can control effect parameters dynamically through physical instrument motion (e.g., tilting the neck up/down for Gain or Wah, pitching/rolling for Chorus speed or Delay feedback) and interact with a touch-driven graphical UI for preset management, parameter mapping, and Tap Tempo.

This design is structured to serve as the blueprint for downstream software engineering agents to implement the firmware autonomously.

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
* **Power & Battery:** AXP2101 PMIC + 500mAh integrated LiPo battery (1.5–3 hours continuous BLE operation).
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

### 3.5 Essential Control Commands

#### A. Preset Switching
* **User Presets (P01 - P50):** Index `0` to `49`
* **Factory Presets (F01 - F50):** Index `50` to `99`
* **SysEx Template:** `8080f0[CRC]00010000010301010403[SLOT_HEX_EXPANDED]000000000000f7`

#### B. Global Volume & Patch Volume
* **Global Volume (0-100):** Modifies input/output master level.
* **Patch Volume (0-100):**
  * `0`: `8080f00F0E0001000000070101040200010200000100000000f7`
  * `50`: `8080f006000001000000070101040200010200000100000302f7`
  * `100`: `8080f00c050001000000070101040200010200000100000604f7`

#### C. Gain Control (AMP / DRV)
* **DRV Gain (AlgID 0, range 0-100):** Maps to `parameters[2][0][val]`
* **AMP Gain (AlgID 0, range 0-100):** Maps to `parameters[3][0][val]`

#### D. Chorus Controls (FX1 / FX2)
* **A-Chorus / B-Chorus Rate (0.1 Hz - 10.0 Hz):**
  * `0.1 Hz`: `8080f0000e00010000000e01010408000100000000000000000000000000000c0d0c0c0c0c030df7`
  * `1.0 Hz`: `8080f0080400010000000e0101040800010000000000000000000000000000000000000800030ff7`
  * `5.0 Hz`: `8080f0050000010000000e0101040800010000000000000000000000000000000000000a000400f7`
* **A-Chorus / B-Chorus Depth (0 - 100):** Maps to AlgID `0`.

#### E. Delay Time & Tap Tempo
* **Pure / Warm / Tape Delay Time (20ms - 1000ms):**
  * Calculated as $t_{\text{ms}} = \frac{60000}{\text{BPM}} \times \text{NoteMultiplier}$
  * Sent via Delay Parameter `parameters[7][1][ms_value]`

---

## 4. Motion Control & 6-Axis IMU Engine Design

### 4.1 IMU Orientation & Sensor Placement
When mounted on the guitar headstock:
* **X-Axis (Pitch):** Neck tilt (pointing the neck towards the ceiling vs floor).
* **Y-Axis (Roll):** Instrument rotation along the neck axis (twisting the face of the guitar up/down).
* **Z-Axis (Yaw):** Body swiveling left/right (performer body turn).

```
                      [HEADSTOCK]
                      +---------+
       Pitch (X)      | ESP32   |   Roll (Y)
       <---^--->      |  IMU    |   <--(O)-->
                      +----+----+
                           |
                           | [NECK]
                           |
```

### 4.2 Signal Processing Pipeline

```
[MPU6886 Raw Accel/Gyro]
       │
       ▼
[Low-Pass Filter (EMA / Alpha = 0.2)]
       │
       ▼
[Complementary Filter (Pitch & Roll Angles)]
       │
       ▼
[Deadband & Hysteresis Mapping]
       │
       ▼
[Parameter Interpolator & Rate Limiter (Max 20 Hz updates)]
       │
       ▼
[BLE Command Queue]
```

#### Complementary Filter Formula
$$\theta_{\text{pitch}} = 0.98 \times (\theta_{\text{pitch}} + \omega_x \cdot \Delta t) + 0.02 \times \text{atan2}(a_y, a_z) \times \frac{180}{\pi}$$
$$\phi_{\text{roll}} = 0.98 \times (\phi_{\text{roll}} + \omega_y \cdot \Delta t) + 0.02 \times \text{atan2}(-a_x, a_z) \times \frac{180}{\pi}$$

### 4.3 Motion Mapping Presets

1. **Preset 1: Neck Tilt Gain Control (Pitch Driven)**
   * **Axis:** X-Axis (Pitch)
   * **Neutral Angle:** $0^\circ$ (Horizontal playing position)
   * **Active Range:** $+10^\circ$ to $+50^\circ$ (Neck up)
   * **Target Parameter:** DRV Gain or AMP Gain ($0 \to 100$)
   * **Effect:** Raising the guitar neck increases overdrive/gain for solos.

2. **Preset 2: Mod-Roll Chorus Controller (Roll Driven)**
   * **Axis:** Y-Axis (Roll)
   * **Active Range:** $-25^\circ$ to $+25^\circ$
   * **Target Parameter:** FX1/FX2 Chorus Depth or Chorus Rate ($0.1\text{Hz} \to 8.0\text{Hz}$)
   * **Effect:** Twisting the guitar face upwards increases modulation speed/depth.

3. **Preset 3: Spatial Reverb / Delay Expression (Pitch + Roll)**
   * **Pitch ($+10^\circ \to +45^\circ$):** Delay Feedback ($10\% \to 85\%$)
   * **Roll ($0^\circ \to +30^\circ$):** Reverb Mix ($10\% \to 70\%$)

4. **Preset 4: Headstock Snap Tap Tempo (Gesture Driven)**
   * **Gesture:** Sharp Z-axis acceleration spike ($> 2.2g$ peak within $80\text{ms}$).
   * **Action:** Measures inter-tap time interval between two consecutive headstock taps to compute BPM and update Delay Time automatically.

---

## 5. Touchscreen HCI & Graphical User Interface Architecture

The 2.0" 320x240 display is divided into three primary views, selectable via top tabs or swipe gestures:

```
+---------------------------------------------------+
| [PERFORM]  |  [MOTION MAP]  |  [SETTINGS / CAL]   |  <- Top Tab Bar
+---------------------------------------------------+
|                                                   |
|  PATCH: P04 - Heavy Lead *                        |  <- Current Patch & Save Status
|                                                   |
|  +-------------------+   +---------------------+  |
|  |  PITCH (Neck UP)  |   |  TAP TEMPO (120BPM) |  |
|  |  GAIN: 78%        |   |  [ TAP HERE ]       |  |
|  +-------------------+   +---------------------+  |
|                                                   |
|  [ PREV PATCH ]                  [ NEXT PATCH ]   |  <- Quick Patch Switching
|                                                   |
+---------------------------------------------------+
```

### 5.1 UI Screen Descriptions

1. **Performance Screen (Main View)**
   * Large, high-contrast display of active Patch Name and Number (e.g. `P12: Scream Solo`).
   * Live Motion Value Bar Graph (displays active Pitch/Roll tilt percentage).
   * Big Touch **TAP TEMPO** Button with visual pulse indicator.
   * Patch Up / Patch Down touch controls.

2. **Motion Mapper Screen**
   * Selector for Active Motion Mapping Profile (e.g., "Neck Up = Gain", "Roll = Chorus Rate", "Pitch = Reverb Feedback").
   * Sensitivity & Deadband Sliders.
   * Motion Toggle Switch (Enable/Disable motion control instantly).

3. **Settings & Calibration Screen**
   * **Calibrate Rest Position:** Calibrates the $0^\circ$ baseline for the current performer stance.
   * **BLE Status & Reconnect Button.**
   * Battery level indicator and signal RSSI strength.

---

## 6. Firmware Software Architecture (ESP32 / FreeRTOS Task Design)

To ensure smooth 60 FPS UI rendering, sub-10ms motion processing, and reliable BLE transmission without packet drops, the firmware is organized into FreeRTOS tasks distributed across the two ESP32 cores:

```
                                  [ ESP32 DUAL CORE ]
             Core 0 (System & Comms)                   Core 1 (HCI & Processing)
        +-------------------------------+        +-------------------------------+
        |  BLE Central Task (Priority 5)|        |   UI / Render Task (Priority 3)|
        |  - Handles GATT Connection    |        |   - LVGL / LovyanGFX Drawing  |
        |  - Manages Queue Transmission |        |   - Touch event handling      |
        +---------------+---------------+        +---------------+---------------+
                        ^                                        ^
                        | (FreeRTOS Queue)                       | (Mutex)
        +---------------+---------------+        +---------------+---------------+
        |  Protocol Task (Priority 4)   |        |   IMU Engine Task (Priority 4)|
        |  - Encodes SysEx + CRC8       |        |   - 100 Hz MPU6886 Sample     |
        |  - Decodes BLE Notifications  |        |   - Complementary Filter      |
        +-------------------------------+        +-------------------------------+
```

### 6.1 Task Table & Priorities

| Task Name | Core | Priority | Frequency | Responsibility |
| :--- | :--- | :--- | :--- | :--- |
| `Task_IMU` | Core 1 | 4 (High) | 100 Hz (10ms) | Reads MPU6886, calculates pitch/roll, applies deadband, detects tap gestures. |
| `Task_GUI` | Core 1 | 3 (Med) | 30–60 Hz | Renders display UI, handles touch inputs, updates real-time sliders. |
| `Task_Protocol` | Core 0 | 4 (High) | Event-Driven | Translates parameter/motion updates into SysEx packets with CRC-8. |
| `Task_BLE` | Core 0 | 5 (Critical)| Event-Driven | Connects to Pocket Master, sends queued GATT writes, receives notification responses. |

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
  * The emulator validates incoming packets: checks for `80 80 F0` header, verifies the CRC-8 SMBus PEC checksum, prints parameter changes to console, and sends ACK notifications back to the ESP32.
* **Option B (Local PocketEdit Web App):** Running `index.html` from `PocketEdit` connected via Web Bluetooth or Virtual USB MIDI (using `loopMIDI` on Windows or `IAC Driver` on macOS) to visually confirm that transmitted commands correctly manipulate the virtual pedal controls.

---

## 8. Autonomous Implementation Roadmap for Coding Agents

Coding agents implementing this system should follow this modular sequence:

```
[Stage 1: Core Drivers & Hardware Support]
   ├── Initialize M5Stack Core2 (Display, Touch, MPU6886, AXP2101 PMIC)
   └── Test IMU data read (Pitch / Roll calculation)

[Stage 2: BLE Central & SysEx Encoder]
   ├── Implement BLE Client scanning for "Sonic Master BLE"
   ├── Implement CRC-8 (SMBus PEC polynomial 0x07) calculation
   └── Verify command transmission (Preset change, Volume, Parameter write)

[Stage 3: Motion Engine & Filtering]
   ├── Implement Complementary Filter for Pitch & Roll
   ├── Build motion-to-parameter interpolation with 20 Hz rate limiting
   └── Build gesture detector for Headstock Snap Tap Tempo

[Stage 4: Touchscreen HCI / UI]
   ├── Implement GUI screens (Performance, Motion Mapper, Calibration)
   └── Wire UI controls to FreeRTOS queues & BLE protocol tasks

[Stage 5: Simulation & End-to-End Verification]
   ├── Verify UI layout in LVGL Simulator / Wokwi
   ├── Test BLE SysEx encoding against Virtual Pocket Master Peripheral
   └── Validate battery management and auto-reconnect logic
```

---

## 9. Summary
This specification provides a complete, self-contained architecture for an off-the-shelf, headstock-mounted ESP32 motion controller for the Sonicake Pocket Master. All hardware specs, BLE SysEx protocol details, CRC checksum algorithms, IMU complementary filtering equations, FreeRTOS task mappings, and Virtual Emulators (Wokwi, LVGL, Python BLE Loopback) are fully defined.
