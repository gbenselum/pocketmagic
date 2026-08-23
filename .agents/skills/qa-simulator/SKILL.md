---
name: qa-simulator
description: >-
  Runbook for Agent-QA executing unit testing, Wokwi hardware simulation, and Python virtual BLE peripheral emulation.
---

# Skill: QA, Native Testing & Virtual Simulation

This skill guides **`Agent-QA`** in executing Epic 5 (`CARD-SIM-501`) and continuous verification across all cards.

## 1. Native C++ Unit Testing
- Run native desktop tests for CRC-8 and packet framing:
  ```bash
  pio test -e native
  ```
- Fast execution on macOS/Linux without compiling the full ESP32 toolchain.

## 2. Python Virtual BLE Peripheral Emulator (`sim/virtual_pedal.py`)
- Emulates the Sonicake Pocket Master pedal on a PC/Mac.
- Advertises as `Sonic Master BLE`.
- Hosts GATT Service `03b80e5a-ede8-4b33-a751-6ce34ec4c700` and Characteristic `7772e5db-3868-4112-a1a9-f2669d106bf3`.
- Logs and decodes all incoming SysEx packets:
  - Validates CRC-8 SMBus PEC checksum.
  - Decodes Module ID, Parameter Index, and Value.
  - Displays simulated pedal screen state in terminal.

## 3. Wokwi ESP32 Hardware Simulator (`sim/wokwi.toml`, `sim/diagram.json`)
- Simulates ESP32 + ILI9341 display + MPU6050/MPU6886 sensor.
- Sliders for Accel X/Y/Z and Gyro X/Y/Z to test neck tilt response in real-time browser preview.
