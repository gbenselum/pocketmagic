# AGENTS.md - Multi-Agent Workspace Guide & Architecture Rules

Welcome to the **PocketMagic** project repository. This workspace contains the firmware design and implementation for an **ESP32-based Headstock Motion & Touchscreen Controller for the Sonicake Pocket Master multi-effects pedal**.

This document defines the agent execution rules, role definitions, architectural constraints, and standard operating procedures for all AI coding agents working in this repository.

---

## 1. Project Overview & Architecture

### 1.1 Goal
Create an out-of-the-box, **zero-soldering** motion controller mounted on a guitar/bass headstock using the **M5Stack Core2 v1.1** (ESP32-D0WDQ6-V3 / S3, 2.0" capacitive touch TFT, MPU6886 6-axis IMU, AXP2101 PMIC). The device connects as a **BLE Central** to the **Sonicake Pocket Master** pedal, transmitting real-time MIDI SysEx commands to control parameters (e.g. Neck Tilt Gain, Tap Tempo BPM).

### 1.2 Multi-Agent Role Breakdown

| Role | Domain / Responsibilities | Assigned Epics / Cards | Primary Skill |
| :--- | :--- | :--- | :--- |
| **`Agent-Platform`** | FreeRTOS dual-core task layout, `M5Unified` initialization, plugin lifecycle management, swipe gesture dispatcher. | Epic 1 (`CARD-CORE-101`, `CARD-CORE-102`) | `platform-core` |
| **`Agent-Comms`** | NimBLE Central GATT client, connection state machine, SysEx packet framing, CRC-8 SMBus PEC calculation, bounded write rate limiter. | Epic 2 (`CARD-BLE-201`, `CARD-BLE-202`) | `comms-sysex` |
| **`Agent-IMU`** | MPU6886 I2C polling (100Hz), Complementary & EMA pitch filtering, resting baseline calibration, Pitch-to-Gain curve mapping. | Epic 3 (`CARD-IMU-301`, `CARD-IMU-302`) | `imu-dsp` |
| **`Agent-UI`** | LVGL 8.x GUI screens (320x240), Mode 1 Tap Tempo engine, Mode 2 Live Gain bar graph, Mode 3 Config & Calibration menu. | Epic 4 (`CARD-UI-401`, `CARD-UI-402`) | `lvgl-ui` |
| **`Agent-QA`** | Native unit testing (`test_crc8`, `test_protocol`), Wokwi simulator setup, Python virtual BLE peripheral test harness. | Epic 5 (`CARD-SIM-501`) | `qa-simulator` |

---

## 2. Core Constraints & Golden Rules

1. **Strict Zero-Soldering Rule:**
   All hardware integrations target commercial off-the-shelf enclosures (M5Stack Core2 / Core2 v1.1 / M5Tough). All peripheral access must use the `M5Unified` library.

2. **FreeRTOS Dual-Core Affinity:**
   - **Core 0 (Comms & Protocol):** `Task_BLE` (GATT client) and `Task_Protocol` (SysEx queue & rate limiting).
   - **Core 1 (Application & UI):** `Task_IMU` (MPU6886 sampling at 100Hz) and `Task_GUI` (LVGL event loop & rendering).
   - Inter-core communication must strictly use FreeRTOS `QueueHandle_t` or thread-safe atomic flags. Never access shared UI or BLE pointers across cores without synchronization.

3. **Rate Limiting & Defensive Clamping:**
   - Motion updates sent over BLE must be strictly rate-limited to **maximum 20 writes/second** (50ms interval).
   - All parameter values must pass through `std::clamp(val, min, max)` before byte encoding.
   - SysEx packet lengths must be validated against `MAX_SYSEX_PAYLOAD_SIZE` (256 bytes).

4. **Memory Management (No Heap Fragmentation):**
   - No dynamic memory allocation (`malloc`, `new`, `std::string` reallocation) inside high-frequency loops (`Task_IMU`, `Task_GUI`, `Task_Protocol`).
   - Use pre-allocated static buffers and fixed-size queues.

---

## 3. GitFlow & Card Execution Workflow

All development is structured around Jira-style task cards located in `.agents/cards/`:

1. **Pick a Card:**
   Read the card file in `.agents/cards/CARD-XXX-YYY.md`. Review dependencies, acceptance criteria, and token budget.

2. **Branching:**
   Create a dedicated feature branch from `develop`:
   ```bash
   git checkout develop
   git pull origin develop
   git checkout -b feature/<epic>-<card-id>-<short-description>
   ```
   *Example:* `git checkout -b feature/core-101-freertos-skeleton`

3. **Implement & Verify:**
   - Use the designated skill in `.agents/skills/<skill-name>/SKILL.md`.
   - Run local verification (unit tests, compilation, or simulation).

4. **Commit & Pull Request:**
   - Write structured commit messages:
     ```text
     feat(core): implement FreeRTOS dual-core skeleton (CARD-CORE-101)
     ```
   - Verify that all acceptance criteria are met before merging into `develop`.

---

## 4. Repository Structure

```text
.
├── .agents/
│   ├── rules/                # Hierarchical workspace rules (C++, FreeRTOS, GitFlow)
│   ├── skills/               # Step-by-step role runbooks
│   └── cards/                # Jira-style task cards with token metrics
├── include/
│   ├── config.h              # Pinouts, task priorities, queue sizes, UUIDs
│   ├── plugins/              # Abstract interfaces (IMotionPlugin, IScreenPlugin)
│   ├── protocol/             # SysEx and BLE GATT data types
│   └── dsp/                  # Motion filter algorithms (EMA, Complementary)
├── src/                      # Source implementation files
├── sim/                      # Wokwi simulation config & Python virtual pedal
├── test/                     # Unit test suites (Native & PlatformIO)
├── AGENTS.md                 # This workspace guide
├── SPECIFICATION.md          # Full architectural blueprint
└── platformio.ini            # PlatformIO build & test environments
```

---

## 5. Token Budgeting Guidelines

Every card has an allocated **Token Budget** reflecting expected LLM context consumption:
* **XS (~5,000 - 12,000 Tokens):** Single header/driver stub or helper.
* **S (~12,000 - 25,000 Tokens):** Discrete feature module, protocol encoder, or screen view.
* **M (~25,000 - 45,000 Tokens):** System engine, multi-file plugin, or driver integration.
* **L (~45,000 - 80,000 Tokens):** Full integration, UI stack, or simulation harness.

Agents must operate with precision, keeping edits modular and avoiding context bloat.

---

# 6. Token Accounting Rules (Mandatory)

Every agent tracks its own consumption. The numbers come from the OpenCode
harness session totals shown at session end. Estimates are forbidden.

## 6.1 Session end — every agent, every session

Append one row to your own ledger:

```bash
python3 scripts/token_ledger.py log <your-agent-name> <input_tokens> <output_tokens> "<what was done>"
```

- Description: 40 characters maximum; the script rejects longer text.
- One row per working session. Never edit or delete existing rows.
- Read-only agents (`plan`, `troubleshoot`, `guidance-agent`) append rows the
  same way through the shell; their edit restriction does not cover this bookkeeping.

## 6.2 Merge to main

The merging agent runs, then commits both files in the merge:

```bash
python3 scripts/token_ledger.py summarize
```

This regenerates `total_tokensconsumed.md` from all seven ledgers and refreshes
the "This application was made with N tokens" line at the bottom of `README.md`.

## 6.3 Pull requests

Every PR description includes a Tokens Consumed table (template:
`.github/pull_request_template.md`). The PR TOTAL must equal the sum of ledger
rows for sessions included in that PR.

## 6.4 Files

| File | Location | Owner |
| :--- | :--- | :--- |
| Per-agent ledger (7 files) | `.agents/agent_<name>_consumed_tokens.md` | That agent only |
| Unified total | `total_tokensconsumed.md` (repo root) | Merging agent, via script |
| README footer | `README.md` last line | Script on summarize |
