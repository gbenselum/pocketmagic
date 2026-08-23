# PocketMagic Feasibility Review (Preplan)

> **Date:** 2026-08-23 — Scope: documentation-only review of SPECIFICATION.md, AGENTS.md,
> all task cards, rules, skills, `platformio.ini`, `ci.yml`, existing stubs. No implementation
> has started. External evidence: Wokwi official docs/issues; PocketEdit reference source.

---

## 1. Verdict

**VIABLE — proceed to planning lock.** Overall architecture maturity 8/10;
pre-plan de-risking completeness was 6/10 at review start, now materially higher after
locating a full reverse-engineered protocol reference (PocketEdit) and correcting two
spec-level errors before any code was written.

The three pillars (motion DSP, BLE MIDI control, LVGL touch UI) are individually
well-understood engineering and collectively sound on the selected hardware.

## 2. Findings Register

| ID | Severity | Finding | Status |
| :-- | :-- | :--- | :--- |
| R1 | CRITICAL | SysEx framing in SPECIFICATION.md 3.2 omits TP/PN/LEN header fields and misstates CRC input scope; protocol had no in-repo evidence | **RESOLVED (docs)** — PocketEdit located; full corrected frame documented in `docs/PROTOCOL.md`; hardware verification moved to CARD-PRE-000 |
| R2 | HIGH | Parameter values assumed freely encodable; reality is table-driven proprietary encoding | **RESOLVED (design)** — CARD-BLE-202 rescoped to table-driven encoder (`docs/PROTOCOL.md` Section 4) |
| R3 | HIGH | Wokwi cannot simulate Bluetooth (verified: unsupported on all chip models; NimBLE init hangs simulator); no M5Stack Core2 board model exists | OPEN — CARD-SIM-501 must be rescoped to display/IMU logic only + `-DWOKWI_BUILD` stub flag |
| R4 | MEDIUM | Python BLE *peripheral* emulator does not work on macOS (dev machine); bleak is central-only, bleno unmaintained | MITIGATION DECIDED — primary emulator becomes a mock-pedal ESP32 sketch; Python variant demoted to Linux-only secondary |
| R5 | HIGH | CI release job will fail: missing `permissions: contents: write` (default GITHUB_TOKEN is read-only on repos created after Feb 2023) | OPEN — fix listed in `docs/SPEC_AMENDMENTS.md` A-08 |
| R6 | MEDIUM | `pio test -e native \|\| echo ...` swallows test failures in CI | OPEN — A-09 |
| R7 | MEDIUM | Task_BLE stack 4096 B too small for NimBLE connect/discovery callbacks | OPEN — A-11 (recommend 8192 B) |
| R8 | LOW | Mounting orientation / axis convention never defined; pitch axis vs neck-tilt mapping depends on clamp direction | OPEN — A-13 (add mounting section before IMU cards execute) |
| R9 | LOW | Repo hygiene: no LICENSE file, no lv_conf.h, no .clang-format, GitFlow branches do not exist yet (single long-named spec branch), battery figure inconsistent (390 vs 500 mAh), "/S3" error in antigravity chart | OPEN — A-12/A-14/A-15 |
| R10 | LOW | No persistence: calibration baseline lost every reboot | OPEN — propose NVS storage in Mode 3 scope |
| R11 | INFO | Power budget unstated (~3–5 h on 390 mAh at 100 Hz + BLE + LCD) | DOCUMENTED — acceptable for POC |
| R12 | INFO | End-to-end latency expectation unset (~60–150 ms motion-to-audio) | DOCUMENTED — fine for gain swells; not a wah |
| R13 | INFO | Security section suggests Passkey pairing; MIDI-BLE pedals typically Just Works; forced pairing risks breaking connectivity | OPEN — A-10 (default: no bonding, optional MAC pinning) |

## 3. Subsystem Feasibility Scorecard

| Subsystem | Feasibility | Notes |
| :--- | :--- | :--- |
| FreeRTOS dual-core architecture | HIGH | Correct topology; stack sizes need tuning (R7) |
| MPU6886 sampling + complementary/EMA pitch | HIGH | Standard DSP; needs mounting convention (R8) |
| PitchGain mapping | HIGH | Device gain params accept exactly 0..100 integer steps — ideal match |
| NimBLE central client | HIGH | UUIDs verified against reference implementation |
| SysEx encoder / CRC-8 | **HIGH now** | Algorithm verified correct; framing corrected; encoder rescoped to tables |
| Tap tempo -> delay time | HIGH | Device supports 20..1000 ms in 1 ms steps; reference semantics extracted |
| LVGL 8.3 UI (3 screens, swipe) | HIGH | Needs lv_conf.h + single-thread LVGL rule made explicit |
| Wokwi simulation | LOW-MEDIUM | Only viable as logic/UI smoke test, never BLE (R3) |
| Mock-pedal ESP32 emulator | HIGH | Recommended replacement for Python-on-macOS (R4) |
| CI/CD pipeline | HIGH | Mechanical fixes required before first tag (R5/R6) |

## 4. Evidence Base

1. **Wokwi Bluetooth support:** official docs peripheral matrix marks Bluetooth unsupported
   for ESP32/S3/C3/C5/C6; wokwi-features issue #225 documents NimBLE init hanging the sim.
   Runtime-varyable MPU6050 inputs exist via Automation Scenarios (IMU-slider concept survives,
   BLE does not).
2. **PocketEdit** (github.com/suckyble/PocketEdit): browser editor for the exact target pedal,
   built from HCI snoop captures. Contains complete decoded payload format doc, CRC-8/SMBus
   implementation identical to our spec snippet, pre-captured command libraries quantified in
   `docs/PROTOCOL.md`, and tap-tempo reference logic. Device name and GATT UUIDs confirmed
   character-for-character.
3. **Git state at review:** single branch `esp32-motion-controller-spec-4213077467574600046`,
   default on origin; no main/develop yet. `.agents/` content fully tracked.

## 5. Decisions Locked During Review (user-confirmed)

1. Framework: **Arduino via PlatformIO stays** (ease-of-integration choice; M5Unified/LVGL/NimBLE ecosystem compatibility prioritized over raw IDF control).
2. Hardware purchase deferred until after plan lock; single-device preference;
   second-board option captured in `requiredhardware.md`.
3. PocketEdit adopted as protocol source of truth; CARD-PRE-000 created to freeze golden vectors.
4. All preplan outputs committed as markdown only; zero implementation files.

## 6. Gate to Implementation

Plan lock requires: (a) amendments in `docs/SPEC_AMENDMENTS.md` reviewed/accepted,
(b) `main` + `develop` branches pushed and protected, (c) CARD-PRE-000 acknowledged as
blocking prerequisite for CARD-BLE-202. After that, execution order per
`.agents/cards/INDEX.md` applies unchanged.
