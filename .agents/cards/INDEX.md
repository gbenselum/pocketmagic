# PocketMagic Multi-Agent Task Cards Dashboard

| Card ID | Epic | Title | Assigned Role | Token Budget | Dependencies | Status |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **`CARD-CORE-101`** | Epic 1: Platform | Project Bootstrap & FreeRTOS Dual-Core Skeleton | `Agent-Platform` | ~18,000 | None | Ready |
| **`CARD-CORE-102`** | Epic 1: Platform | Plugin Manager & Swipe Gesture Framework | `Agent-Platform` | ~16,000 | `CARD-CORE-101` | Pending |
| **`CARD-BLE-201`** | Epic 2: Comms | Sonicake BLE Central Client Plugin | `Agent-Comms` | ~30,000 | `CARD-CORE-102` | Pending |
| **`CARD-BLE-202`** | Epic 2: Comms | SysEx Packet Encoder & CRC-8 SMBus Checksum | `Agent-Comms` | ~22,000 | `CARD-CORE-102` | Pending |
| **`CARD-IMU-301`** | Epic 3: IMU | MPU6886 Driver & Pitch Motion Engine | `Agent-IMU` | ~25,000 | `CARD-CORE-102` | Pending |
| **`CARD-IMU-302`** | Epic 3: IMU | PitchGain Motion Plugin (POC Mode 2) | `Agent-IMU` | ~20,000 | `CARD-IMU-301` | Pending |
| **`CARD-UI-401`** | Epic 4: UI | POC Mode 1: Tap Tempo Touch Screen Plugin | `Agent-UI` | ~28,000 | `CARD-CORE-102` | Pending |
| **`CARD-UI-402`** | Epic 4: UI | POC Mode 2: Motion Gain & Mode 3 Config Screens | `Agent-UI` | ~32,000 | `CARD-UI-401` | Pending |
| **`CARD-SIM-501`** | Epic 5: QA/Sim | Wokwi Simulator & Python Virtual BLE Peripheral | `Agent-QA` | ~20,000 | `CARD-BLE-201`, `CARD-BLE-202` | Pending |

## Total Estimated Workload
- **Total Token Budget:** ~211,000 Tokens across all 9 task cards.
- **Execution Order:**
  1. `CARD-CORE-101` -> `CARD-CORE-102` (Platform Foundation)
  2. Parallel Streams:
     - Stream A (Comms): `CARD-BLE-201`, `CARD-BLE-202`
     - Stream B (IMU DSP): `CARD-IMU-301` -> `CARD-IMU-302`
     - Stream C (Touch UI): `CARD-UI-401` -> `CARD-UI-402`
  3. Integration & QA: `CARD-SIM-501`
