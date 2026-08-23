# SPEC & Infrastructure Amendments — Proposed, Pending Approval

> Preplan stage: nothing here has been applied to `SPECIFICATION.md`, `platformio.ini`,
> or `.github/workflows/ci.yml`. Each amendment lists target, rationale (finding ID from
> `docs/FEASIBILITY_REVIEW.md`), and the exact proposed change. On approval these become
> part of the corresponding card's execution.

## A-01 — Correct SysEx framing in SPECIFICATION.md Section 3.2 [R1]
Replace the 5-field frame with the verified 7-field layout including Total Packets,
Current Packet, and Payload Length; restate CRC input scope as TP+PN+LEN+payload
(de-expanded). Full replacement text available in `docs/PROTOCOL.md` Sections 1-2.
**Target:** SPECIFICATION.md 3.2/3.3. **Blocks:** CARD-BLE-202.

## A-02 — Rescope CARD-BLE-202 to table-driven value encoding [R2]
Delete the free-form `encodeParamChange(moduleId, paramId, float)` requirement;
replace with quantize-to-nearest-discrete-value lookup consuming
`include/protocol/pocket_master_tables.hpp` produced by CARD-PRE-000.
Keep CRC-8 unit tests unchanged (algorithm verified correct).

## A-03 — Widen Mode 1 BPM range to match device [R2 evidence]
Device supports delay times 20..1000 ms => tempos down to 20 BPM at 1/4 division.
Change UI-401 "40..300 BPM" to "20..300 BPM" with hard clamp of the final delay
value to 20..1000 ms after note-multiplier application (mirrors PocketEdit behavior).

## A-04 — Tighten MAX_SYSEX_PAYLOAD_SIZE guidance [R1]
Largest real command is 19 logical payload bytes (~50-byte frame). Change
`include/config.h` comment and BLE-202 bound check to a 32-logical-byte limit with an
assertion that frame length <= negotiated MTU - 3. Retain write-without-response.

## A-05 — Rescope CARD-SIM-501 [R3/R4]
1. Primary emulator deliverable becomes **mock-pedal ESP32 sketch** (`sim/mock_pedal/`):
   NimBLE peripheral advertising `Sonic Master BLE` with the verified service/characteristic,
   decoding incoming frames, logging parameter changes over serial. Runs identically on
   macOS/Linux/CI hardware-in-loop.
2. Python `virtual_pedal.py` demoted to Linux/BlueZ-only secondary.
3. Wokwi limited to LVGL screen rendering + DSP logic smoke tests via generic ESP32 +
   ILI9341 + MPU6050 diagram; requires `-DWOKWI_BUILD` flag that stubs BLE init
   (NimBLE startup hangs the simulator). Remove "Wokwi CLI Tests" from the CI diagram in
   SPECIFICATION.md Section 11 or mark Phase 2.

## A-06 — HAL seam decision for simulation [R3]
To let UI/DSP code run under Wokwi without M5Unified, introduce minimal
`IImuSource` / `IDisplay` interfaces during CARD-CORE-101/102 (two tiny headers,
static binding). If rejected, drop the Wokwi pillar entirely rather than half-support it.

## A-07 — Add mounting/orientation section before IMU work begins [R8]
New SPECIFICATION.md Section 2.4: physical clamp orientation, which sensor axis maps to
neck tilt, gyro axis used by the complementary filter, baseline-relative range semantics
(+10..+50 deg relative to calibrated neutral), vibration damping note.

## A-08 — Fix CI release permissions [R5]
Add to `.github/workflows/ci.yml`:
```yaml
permissions:
  contents: read
jobs:
  release:
    permissions:
      contents: write   # required by softprops/action-gh-release on modern default tokens
```
Without this, the first tagged push produces a failed release job (HTTP 403).

## A-09 — Stop swallowing native test failures [R6]
Remove `|| echo "No native tests executed yet"` from the validate job. Commit a placeholder
`test/native/test_crc8/test_main.cpp` in CORE-101 so `pio test -e native` always finds tests.
Also: deduplicate the two identical `setup-python@v5` steps; add
`concurrency: { group: ci-${{ github.ref }}, cancel-in-progress: true }`;
scope cache to `~/.platformio/packages`, `~/.platformio/platforms`, `~/.platformio/.cache`.

## A-10 — Pairing policy correction [R13]
SPECIFICATION.md Section 6.1: make no-bonding/Just Works the default; demote Passkey/
Numeric Comparison to future option; add post-first-pairing MAC pinning as the practical
anti-hijack measure. Rationale: MIDI-BLE peripherals conventionally skip bonding; forced
security risks breaking connectivity with the pedal.

## A-11 — Raise Task_BLE stack [R7]
`include/config.h`: `STACK_SIZE_BLE_TASK = 8192`. Keep HWM checks from CARD-CORE-101 as
reported acceptance output.

## A-12 — Repo bootstrap bundle [R9]
In CARD-CORE-101 scope: add root `LICENSE` (MIT text already drafted in spec),
`lv_conf.h` (LV_COLOR_16, tick source, heap policy), `.clang-format`,
`-Wall -Wextra` in both platformio envs (gitflow rules demand warning-free builds but
flags are absent today), and decide `-fno-exceptions -fno-rtti` per embedded_cpp rules.

## A-13 — Branch topology bootstrap [R9]
Before any card executes: create protected `main` from current tip (done alongside these
docs), create `develop` from `main`, delete/rename the long-named agent branch after merge,
set origin default branch to `main`. All future feature branches follow
`feature/<epic>-<card-id>-<slug>` per `.agents/rules/gitflow_workflow.md`.

## A-14 — Documentation consistency fixes [R9]
1. antigravity_chart.md: remove "/ S3" from Core2 CPU description (Core2 is not ESP32-S3).
2. SPECIFICATION.md 2.3 battery: state 390 mAh integrated (500 mAh figure applies only
   with M5GO Bottom2, which A-12/requiredhardware excludes).
3. Sync SPECIFICATION.md 11.1 sample workflow with the actual `ci.yml` (they diverge:
   unittest-discover vs pio test) so downstream agents do not implement against stale YAML.

## A-15 — Persistence + power notes (post-POC friendly)
1. Add NVS persistence of calibration baseline + last active mode; write only on user
   action (no wear concern). Slot into Mode 3 config screen scope.
2. Add one paragraph to SPECIFICATION.md: expected battery life ~3-5 h at full load;
   latency expectation ~60-150 ms motion-to-audio; POC success criteria = gain swells,
   not wah-class responsiveness.

## A-16 — Framework confirmation (user decision recorded)
Keep Arduino framework via PlatformIO (`espressif32 @ ^6.x`) as chosen for ecosystem
compatibility ease. Record rationale here so future agents do not relitigate:
M5Unified + LVGL 8.3 + NimBLE-Arduino integration is lowest-friction on Arduino core;
ESP-IDF native would buy finer power/RTOS control at the cost of fighting library examples.
