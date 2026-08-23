# Required Hardware — Grocery List (Preplan)

> Decision context (user-confirmed, 2026-08-23): purchase deferred until plan lock;
> single-device preferred; a second board is **optional**, not required, because the real
> Sonicake Pocket Master pedal is already owned. Prices are approximate street prices.

## 1. Required

| # | Item | Qty | Est. Price | Purpose / Notes |
| :- | :--- | :-- | :--- | :--- |
| 1 | **M5Stack Core2 — board revision decision required, see Section 1a** | 1 | ~$47 | ESP32-D0WDQ6-V3, 2.0" capacitive touch 320x240 (ILI9342C + FT6336U), MPU6886 IMU, 16 MB flash / 8 MB PSRAM, USB-C, includes 20 cm USB-C cable. Functionally identical across revisions for this project because all hardware access goes through M5Unified. |
| 2 | USB-C **data** cable | 0–1 | ~$5–10 (a 20 cm data cable ships in the box) | Spare/longer cable for bench flashing comfort. Many cheap cables are charge-only; verify data lines before blaming tooling. |

**Required subtotal: ~$47–55**

### 1a. Board Revision Decision (validated against official M5Stack sources, 2026-08-23)

| | Original Core2 (`K010`) | Core2 v1.1 (`K010-V11`) |
| :--- | :--- | :--- |
| Shop listing (official) | shop.m5stack.com/products/m5stack-core2-esp32-iot-development-kit | shop.m5stack.com/products/m5stack-core2-esp32-iot-development-kit-v1-1 |
| Availability | In stock ($46.90 at validation time) | Marked **[EOL]** on official shop; check resellers (Pi Hut, Core Electronics, Switch Science) |
| PMU | AXP192 | AXP2101 + INA3221 current meter |
| Power LED | Green | Blue |
| RTC backup battery | None | Yes (accurate timing after power-off) |
| USB serial chip | CP2104 or CH9102F (varies) | CH9102F |
| Weight | 54.9 g | 45.1 g |
| Battery (official spec) | 500 mAh @ 3.7 V | 500 mAh @ 3.7 V |

Impact on this project: **none functionally.** SPECIFICATION golden rule #1 mandates all
hardware access via M5Unified, which auto-detects AXP192 vs AXP2101 and drives both
revisions transparently (`platform-core` skill already documents "PMIC (AXP192/AXP2101)").
No card, config constant, or driver code depends on the PMIC variant.

Recommendation (DevOps/engineering view):
1. If a v1.1 reseller ships to your country near list price: buy v1.1 — matches the
   documented target exactly, ~10 g lighter for headstock mounting, and the INA3221
   fuel-gauge gives cleaner battery readings for the Mode 3 status screen.
2. Otherwise buy the in-stock original K010 from the linked page and we amend
   SPECIFICATION Section 2.3 to name AXP192. Do not pay a premium hunting EOL stock;
   the POC gains nothing from it.

Setup note for either revision (macOS): first flash may require the CH9102F or CP210x
USB-serial driver; verify `ls /dev/tty.usbserial*` / `ls /dev/tty.usbmodem*` before blaming
the toolchain.

## 2. Required for Mounting (choose one strategy)

| # | Item | Qty | Est. Price | Notes |
| :- | :--- | :-- | :--- | :--- |
| 3a | Guitar headstock clip / tuner-style clamp + elastic strap or velcro | 1 | ~$5–12 | Simplest zero-solder option. Device weighs 54.9 g (K010) / 45.1 g (v1.1); strap safety matters on a headstock. |
| 3b | *Alternative:* small camera/GoPro-style mount adapter plate bonded to a case | 1 | ~$8–15 | More rigid, better for repeatable tilt angles; adds weight. |
| 4 | Thin foam tape / padding | 1 | ~$3 (likely owned) | Vibration damping between device and headstock — measurably reduces gyro noise from the instrument body. |

## 3. Optional (recommended, defer decision)

| # | Item | Qty | Est. Price | Purpose |
| :- | :--- | :-- | :--- | :--- |
| 5 | Any cheap ESP32 dev board (ESP32 DevKitC / WROOM-32) | 0–1 | ~$6–10 | Stand-in "mock pedal" peripheral for regression tests when the real Pocket Master is not reachable (CI-style automated runs, development away from the rig). NOT needed for the POC itself — the real pedal covers E2E testing. Decide after first hardware milestone. |
| 6 | Android phone with nRF Connect (free app) | 0 (likely owned) | $0 | Golden-vector capture for CARD-PRE-000: HCI snoop log or GATT observer while PocketEdit drives the pedal. An nRF52 Sniffer dongle (~$40) is overkill at this stage. |

## 4. Explicitly Not Needed

| Item | Reason |
| :--- | :--- |
| M5GO Bottom2 battery base | Both Core2 revisions already integrate a 500 mAh LiPo (official spec); the bottom roughly doubles headstock weight for no POC benefit. |
| M5StickC PLUS2 | No touchscreen — violates core product concept. |
| M5Tough | Heavier (~70 g), same electronics; ruggedness irrelevant here. |
| LilyGO T-Display-S3 Touch | IMU not integrated; violates zero-solder integration rule. |
| USB Bluetooth 5.0 dongle (PocketEdit README suggestion) | That is for the PC editor; the Core2 has its own BLE radio as central. |
| Soldering equipment of any kind | Project rule: zero soldering. |

## 5. Budget Summary

| Scenario | Total |
| :--- | :--- |
| Minimum viable POC (items 1+2, existing cable, strap from drawer) | **~$50–60** |
| Comfortable setup (+ mounting hardware + foam) | ~$65–80 |
| With optional mock-pedal board added later | +$6–10 |

## 6. Procurement Notes

1. Revision decision per Section 1a: prefer v1.1 (`K010-V11`) if a reseller has it at fair
   price; otherwise the in-stock original (`K010`) is fully acceptable — record whichever
   is purchased so SPECIFICATION Section 2.3 can name the correct PMIC (AXP2101 vs AXP192).
2. The box includes a 20 cm USB-C cable and hex wrench; only buy a spare cable if you
   want more reach on the bench.
3. On arrival: run M5Stack's factory burn-in sketch once, then flash a trivial M5Unified
   hello-world and confirm `M5.getBattery()` / power APIs behave before starting
   CARD-CORE-101.
