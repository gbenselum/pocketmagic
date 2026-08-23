# CARD-BLE-202: SysEx Packet Encoder & CRC-8 SMBus Checksum

- **Epic:** Epic 2: Communication & Protocol Plugins
- **Priority:** High
- **Assigned Role:** `Agent-Comms`
- **Skill:** `comms-sysex`
- **Feature Branch:** `feature/ble-202-sysex-encoder`
- **Dependencies:** `CARD-CORE-102`
- **Token Budget:** ~22,000 Tokens

## 1. Description
Implement the SysEx packet encoding engine for Sonicake Pocket Master. Encapsulates commands in the required frame structure `[80 80 F0] [CRC_HI CRC_LO] [EXPANDED_PAYLOAD] [F7]` and computes the CRC-8 SMBus PEC checksum (`0x07` polynomial).

## 2. Technical Requirements
1. Implement `calculate_smbus_crc8(const uint8_t* data, size_t len)` with lookup table or bitwise algorithm.
2. Implement nibble expansion: split each payload byte into high/low nibbles.
3. Build encoders for:
   - Module Parameter Write: `encodeParamChange(uint8_t moduleId, uint8_t paramId, float value)`
   - Tap Tempo / Delay Time Write: `encodeDelayTime(uint16_t delayTimeMs)`
   - Preset Switch: `encodePresetChange(uint8_t presetIndex)`
4. Maximum payload size boundary verification (`MAX_SYSEX_PAYLOAD_SIZE = 256`).

## 3. Acceptance Criteria
- [ ] Native C++ unit tests (`test_crc8`) pass with 100% vector accuracy against reference packets.
- [ ] Parameter value clamping (`std::clamp(val, min, max)`) prevents malformed writes.
- [ ] Zero dynamic memory allocation during packet framing.
