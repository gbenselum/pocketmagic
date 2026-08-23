# CARD-SIM-501: Wokwi Simulator & Python Virtual BLE Peripheral

- **Epic:** Epic 5: Simulation & Testing Plugins
- **Priority:** Medium
- **Assigned Role:** `Agent-QA`
- **Skill:** `qa-simulator`
- **Feature Branch:** `feature/sim-501-virtual-pedal-wokwi`
- **Dependencies:** `CARD-BLE-201`, `CARD-BLE-202`
- **Token Budget:** ~20,000 Tokens

## 1. Description
Provide complete virtual simulation infrastructure enabling end-to-end testing without physical pedal or ESP32 hardware:
1. Python BLE Peripheral emulator (`sim/virtual_pedal.py`) running on PC/Mac to simulate `Sonic Master BLE`.
2. Wokwi hardware simulation configuration (`sim/diagram.json` and `sim/wokwi.toml`).

## 2. Technical Requirements
1. **Python BLE Emulator (`sim/virtual_pedal.py`):**
   - Advertise device name `Sonic Master BLE`.
   - Host GATT Characteristic `7772e5db-3868-4112-a1a9-f2669d106bf3`.
   - Parse incoming SysEx frames, compute and verify CRC-8 SMBus PEC.
   - Print human-readable parameter changes (e.g. `[OVERDRIVE] Gain = 78%`, `[DELAY] Time = 450ms (133 BPM)`).
2. **Wokwi Simulator Config:**
   - ESP32 board definition with connected ILI9341 display and MPU6050/MPU6886 I2C sensor.
   - Interactive sliders for motion tilt simulation.

## 3. Acceptance Criteria
- [ ] Python script successfully pairs with ESP32 or test client and decodes SysEx commands in real time.
- [ ] Invalid CRC-8 checksum packets are detected and flagged in error logs.
- [ ] Wokwi simulation loads firmware and visualizes screen output.
