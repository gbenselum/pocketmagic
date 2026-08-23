# Required Hardware — Grocery List (Preplan)

> Decision context (user-confirmed, 2026-08-23): purchase deferred until plan lock;
> single-device preferred; a second board is **optional**, not required, because the real
> Sonicake Pocket Master pedal is already owned. Prices are approximate street prices.

## 1. Required

| # | Item | Qty | Est. Price | Purpose / Notes |
| :- | :--- | :-- | :--- | :--- |
| 1 | **M5Stack Core2 v1.1** (K134-V11) | 1 | ~$50 | Main controller. ESP32-D0WDQ6-V3, 2.0" capacitive touch 320x240, MPU6886 IMU, AXP2101 PMIC, built-in 390 mAh battery, 16 MB flash. Matches SPECIFICATION.md Section 2.3 and every card/skill assumption. |
| 2 | USB-C **data** cable | 1 | ~$5–10 (likely owned) | Flashing + serial monitor. Many USB-C cables are charge-only; verify data lines before blaming tooling. |

**Required subtotal: ~$55**

## 2. Required for Mounting (choose one strategy)

| # | Item | Qty | Est. Price | Notes |
| :- | :--- | :-- | :--- | :--- |
| 3a | Guitar headstock clip / tuner-style clamp + elastic strap or velcro | 1 | ~$5–12 | Simplest zero-solder option. Core2 weighs ~52 g (+ bottom if attached); strap safety matters on a headstock. |
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
| M5GO Bottom2 battery base | Core2 v1.1 already has integrated 390 mAh; bottom doubles headstock weight for no POC benefit. |
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

1. Buy the **v1.1** variant specifically (AXP2101 PMIC). The original Core2 uses AXP192;
   M5Unified handles both, but skills/docs in this repo assume v1.1.
2. Verify the seller ships the correct regional SKU with EU/US plug for the charger —
   or charge from any USB-C source; the device charges standalone via its port.
3. On arrival: run M5Stack factory burn-in once, then confirm firmware version supports
   the v1.1 PMIC before starting CARD-CORE-101.
