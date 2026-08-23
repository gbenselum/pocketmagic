# CARD-BLE-201: Sonicake BLE Central Client Plugin

- **Epic:** Epic 2: Communication & Protocol Plugins
- **Priority:** High
- **Assigned Role:** `Agent-Comms`
- **Skill:** `comms-sysex`
- **Feature Branch:** `feature/ble-201-central-client`
- **Dependencies:** `CARD-CORE-102`
- **Token Budget:** ~30,000 Tokens

## 1. Description
Build the `NimBLE` Central client that scans for the Sonicake Pocket Master pedal (`Sonic Master BLE`), connects to GATT Service `03b80e5a-ede8-4b33-a751-6ce34ec4c700`, discovers Characteristic `7772e5db-3868-4112-a1a9-f2669d106bf3`, and handles connection state transitions.

## 2. Technical Requirements
1. Implement `NimBLEClient` callbacks for Connect, Disconnect, and Passkey security if needed.
2. Background scan filter matching device name `Sonic Master BLE`.
3. Auto-reconnect loop with exponential backoff on connection drop.
4. Consume queued packets from `xBleWriteQueue` and execute `writeValue(..., false)` (write without response) to optimize latency.
5. Rate limit writes to max 20 packets/sec (50ms period).

## 3. Acceptance Criteria
- [ ] Connects automatically to simulated or physical `Sonic Master BLE` device within 3 seconds of discovery.
- [ ] Automatically recovers and reconnects after simulated BLE link loss.
- [ ] Rate limiter caps outbound writes strictly to <= 20 Hz.
