---
name: comms-sysex
description: >-
  Runbook for Agent-Comms implementing NimBLE Central GATT client, Sonicake SysEx packet encoding, CRC-8 SMBus PEC calculation, and rate limiting.
---

# Skill: Sonicake BLE & SysEx Protocol Engine

This skill guides **`Agent-Comms`** in executing Epic 2 cards (`CARD-BLE-201`, `CARD-BLE-202`).

## Protocol Details & Framing
Sonicake Pocket Master requires MIDI SysEx frames wrapped in a 2-byte header:
`[80 80] [F0] [CRC_HIGH] [CRC_LOW] [PAYLOAD_EXPANDED] [F7]`

1. **CRC-8 SMBus Calculation:**
   - Polynomial: $x^8 + x^2 + x^1 + 1$ (`0x07`).
   - Initial seed: `0x00`.
   - Computed on raw (unexpanded) payload bytes.
2. **Nibble Expansion:**
   - Raw bytes are split into high and low nibbles (e.g. `0xC5` -> `0x0C`, `0x05`).
   - SysEx boundary values `0xF0` and `0xF7` remain untouched as framing delimiters.
3. **BLE GATT Service & Characteristic:**
   - Device Name Filter: `Sonic Master BLE`
   - Service UUID: `03b80e5a-ede8-4b33-a751-6ce34ec4c700`
   - Characteristic UUID: `7772e5db-3868-4112-a1a9-f2669d106bf3` (Write Without Response / Notify)

## Rate Limiting & Queueing
- Maintain a thread-safe write queue `xBleWriteQueue`.
- Enforce token bucket rate limiting (max 20 writes/second, 50ms interval).
- Provide auto-reconnect state machine with exponential backoff.

## Verification Steps
- Run CRC-8 unit tests against known Pocket Master SysEx packets.
- Ensure write queue does not overflow or drop critical preset switch commands.
