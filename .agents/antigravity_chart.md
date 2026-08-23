# PocketMagic Multi-Agent Architecture & Dependency Charts

## 1. Multi-Agent Development Workflow Architecture

```mermaid
flowchart TD
    subgraph RepoInfrastructure [Repository Infrastructure]
        CONF[platformio.ini + CMake]
        AGENTS[AGENTS.md & .agents/ Rules]
        SKILLS[.agents/skills/ Specialized Runbooks]
        CARDS[.agents/cards/ Jira Token Cards]
    end

    subgraph AgentRoles [Specialized Agent Roles]
        AP[Agent-Platform: Core & FreeRTOS Skeleton]
        AC[Agent-Comms: BLE GATT & SysEx CRC8]
        AI[Agent-IMU: MPU6886 & Pitch DSP]
        AU[Agent-UI: LVGL 320x240 Swipe & Tap]
        AQ[Agent-QA: Wokwi & Python Virtual BLE]
    end

    subgraph GitFlowBranches [GitFlow Branching]
        DEV[develop branch]
        F1[feature/core-101-freertos-skeleton]
        F2[feature/ble-201-central-client]
        F3[feature/imu-301-mpu6886-engine]
        F4[feature/ui-401-tap-tempo]
        F5[feature/sim-501-virtual-pedal-wokwi]
        MAIN[main release branch]
    end

    RepoInfrastructure --> AgentRoles
    AP --> F1 --> DEV
    AC --> F2 --> DEV
    AI --> F3 --> DEV
    AU --> F4 --> DEV
    AQ --> F5 --> DEV
    DEV --> MAIN
```

---

## 2. Card Execution Order & Dependency Graph

```mermaid
flowchart TD
    CORE101["[CARD-CORE-101]<br>Project Bootstrap & FreeRTOS Skeleton<br><i>~18k Tokens</i>"] --> CORE102["[CARD-CORE-102]<br>Plugin Manager & Swipe Framework<br><i>~16k Tokens</i>"]
    
    CORE102 --> BLE201["[CARD-BLE-201]<br>NimBLE Central Client<br><i>~30k Tokens</i>"]
    CORE102 --> BLE202["[CARD-BLE-202]<br>SysEx Encoder & CRC-8 SMBus<br><i>~22k Tokens</i>"]
    CORE102 --> IMU301["[CARD-IMU-301]<br>MPU6886 Driver & Pitch Engine<br><i>~25k Tokens</i>"]
    CORE102 --> UI401["[CARD-UI-401]<br>Mode 1: Tap Tempo Plugin<br><i>~28k Tokens</i>"]
    
    IMU301 --> IMU302["[CARD-IMU-302]<br>PitchGain Motion Plugin<br><i>~20k Tokens</i>"]
    UI401 --> UI402["[CARD-UI-402]<br>Mode 2 Gain & Mode 3 Config<br><i>~32k Tokens</i>"]
    
    BLE201 --> SIM501["[CARD-SIM-501]<br>Virtual BLE Peripheral & Wokwi<br><i>~20k Tokens</i>"]
    BLE202 --> SIM501
```

---

## 3. Epic, Role & Token Metric Matrix

| Card ID | Epic | Title | Assigned Role | Primary Skill | Token Budget | Dependencies | Status |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **`CARD-CORE-101`** | Epic 1: Platform | Bootstrap & FreeRTOS Dual-Core Task Skeleton | `Agent-Platform` | `platform-core` | ~18,000 | None | **Ready** |
| **`CARD-CORE-102`** | Epic 1: Platform | Plugin Manager & Swipe Gesture Navigation | `Agent-Platform` | `platform-core` | ~16,000 | `CARD-CORE-101` | Pending |
| **`CARD-BLE-201`** | Epic 2: Comms | Sonicake BLE Central Client Plugin | `Agent-Comms` | `comms-sysex` | ~30,000 | `CARD-CORE-102` | Pending |
| **`CARD-BLE-202`** | Epic 2: Comms | SysEx Packet Encoder & CRC-8 SMBus PEC | `Agent-Comms` | `comms-sysex` | ~22,000 | `CARD-CORE-102` | Pending |
| **`CARD-IMU-301`** | Epic 3: IMU | MPU6886 Driver & Pitch Motion Engine | `Agent-IMU` | `imu-dsp` | ~25,000 | `CARD-CORE-102` | Pending |
| **`CARD-IMU-302`** | Epic 3: IMU | PitchGain Motion Plugin (POC Mode 2) | `Agent-IMU` | `imu-dsp` | ~20,000 | `CARD-IMU-301` | Pending |
| **`CARD-UI-401`** | Epic 4: UI | POC Mode 1: Tap Tempo Touch Screen Plugin | `Agent-UI` | `lvgl-ui` | ~28,000 | `CARD-CORE-102` | Pending |
| **`CARD-UI-402`** | Epic 4: UI | Mode 2 Motion Gain & Mode 3 Config Screens | `Agent-UI` | `lvgl-ui` | ~32,000 | `CARD-UI-401` | Pending |
| **`CARD-SIM-501`** | Epic 5: QA/Sim | Wokwi Simulator & Python Virtual BLE Peripheral | `Agent-QA` | `qa-simulator` | ~20,000 | `CARD-BLE-201`, `CARD-BLE-202` | Pending |

### Summary Metrics
- **Total Cards:** 9
- **Total Workload:** ~211,000 Tokens
- **Target Hardware:** M5Stack Core2 v1.1 (ESP32-D0WDQ6-V3 / S3, MPU6886, AXP2101, ILI9342C TFT)
- **Target Peripheral:** Sonicake Pocket Master (`Sonic Master BLE`)
