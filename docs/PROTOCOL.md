# Sonicake Pocket Master — BLE MIDI Protocol Reference (Preplan Draft)

> **Status:** Preplan documentation. Not implemented. Extracted from the reverse-engineered
> reference implementation [PocketEdit](https://github.com/suckyble/PocketEdit)
> (`tools/SYSEX_PAYLOAD_FORMAT.md`, `index.html` `COMMAND_LIBRARY_DATA`), inspected on 2026-08-23.
> Golden-vector verification against physical hardware is tracked in `CARD-PRE-000`.

---

## 1. Corrected Frame Layout

The frame below **supersedes Section 3.2 of SPECIFICATION.md**, which omitted three
header fields (Total Packets, Current Packet, Payload Length) present in every real message.

```
[80][80][F0][CRC_H][CRC_L][TP_H][TP_L][PN_H][PN_L][LEN_H][LEN_L][PAYLOAD...][F7]
 |   |   |    |      |      |      |      |      |      |        |          |     |
 |   |   |    |      |      +------+------+- TP/PN/LEN expanded (6 bytes) -+     |     |
 |   |   |    +------+-- CRC-8/SMBus, nibble-expanded (2 bytes)                  |     |
 |   |   +-- SysEx start                                                         |     |
 |   +-- MIDI BLE timestamp header (always 80 80 on this device)                 |     |
 +-- (pair)                                                       Payload, nibble-exp |
                                                                    SysEx end --------+
```

| Field | Bytes (logical) | Notes |
| :---- | :---- | :---- |
| BLE header | `80 80` | Fixed MIDI timestamp pair. Stripped on USB transport. |
| SysEx start | `F0` | |
| CRC | 1 logical byte, expanded to 2 | See Section 2. |
| Total Packets (TP) | 1 logical byte | `01` for all single-packet commands. |
| Current Packet (PN) | 1 logical byte | `00` for first packet. |
| Payload Length (LEN) | 1 logical byte | Logical payload byte count. Transmitted size = LEN x 2. |
| Payload | LEN bytes, expanded to 2xLEN | Starts with fixed prefix `01 01` (commands) — see Section 3. |
| SysEx end | `F7` | |

**Largest known command:** Save Preset (`040A`), LEN = 19 logical bytes.
**Worst-case full frame:** ~50 transmitted bytes — fits a single GATT write-without-response
at any negotiated MTU >= 53. PocketEdit transmits the entire frame in one
`writeValueWithoutResponse` call with no fragmentation. The `MAX_SYSEX_PAYLOAD_SIZE = 256`
bound in `include/config.h` is therefore generous; a tighter bound of 32 logical bytes is
realistic for this device.

## 2. CRC-8/SMBus PEC

* Polynomial `0x07`, seed `0x00` — identical to the C implementation already in
  SPECIFICATION.md Section 3.3. That snippet is **correct and verified** against PocketEdit's JS.
* Input scope (**correction vs. spec**): computed over the *de-expanded* byte string starting at
  the Total Packets field through the end of the payload — i.e., TP + PN + LEN + PAYLOAD.
  It does **not** cover the `80 80 F0` header and does **not** cover only the payload.
* De-expansion (as performed by PocketEdit): remove the leading zero nibble of every
  expanded byte pair (regex `0(.)` -> `$1`).
* Result byte is nibble-expanded for transmission (e.g., `0xC5` -> `0C 05`).

## 3. Payload Structure and Command Catalog

Payload = `[01 01] [CMD_TYPE] [BODY...]` for outgoing data commands.
Outgoing query commands use prefix `[01 02]`; incoming responses use `03xx` types
and/or prefix `01 02`.

### 3.1 Commands required for the POC

| Cmd | Name | LEN | Body layout (logical offsets after prefix+cmd) | POC use |
| :-- | :--- | :-- | :--- | :--- |
| `0408` | Parameter Value Set | 14 | `MOD @0`, `ALGID @9`, `VALUE_BLOCK @22` (6 bytes) | Mode 1 delay time, Mode 2 gain |
| `0403` | Preset Select | 6 | `PRESET_IDX @0`, padding | Future preset switching |
| `0409` | Module On/Off | 10 | `MOD @0`, zeros, `STATE @18` (`01`=on) | Bypass toggle (optional) |
| `0101` | Global Setting | 10 | `GROUP @0`, `SUBID @2`, value block | Global/preset volume |

All offsets above are logical byte offsets inside BODY (after the 4-byte
`0101`+CMD header), expressed post-de-expansion. Multi-byte fields are themselves
nibble-expanded on the wire.

### 3.2 Query commands (App -> Device), prefix `0102`

| Payload tail | Purpose |
| :--- | :--- |
| `0204 00` | Request all preset names |
| `0204 01` | Request current preset state dump |
| `0204 03` | Request current preset number |
| `0201 00` | Request global settings |

POC firmware does not need to send queries, but should tolerate unsolicited
response packets arriving after connection (the pedal emits dumps to editors).

### 3.3 Device -> Host notifications

* Success ACK payload `0104 0008 0000`; failure NACK `0104 0008 0001`.
* State dumps arrive as multi-packet sequences routed by context, terminated by an
  empty-payload packet (`...0105 0104 0002 00000000 F7` pattern).
* **Firmware policy (POC):** subscribe to Notify on the characteristic; log and discard
  non-ACK packets. ACK correlation is a post-POC feature.

## 4. Parameter Value Encoding — CRITICAL DESIGN CONSTRAINT

Parameter values are **not** plain integers on the wire. Each (module, algId, discrete value)
maps to a unique 6-byte value block with a non-linear, proprietary encoding. PocketEdit ships
pre-captured command tables rather than computing values.

Verified table inventory (extracted from `COMMAND_LIBRARY_DATA.parameters`):

| Module | AlgID | Parameter | Discrete values | Range |
| :--- | :--- | :--- | :--- | :--- |
| 2 DRV | 0 | Gain | 101 | 0..100 |
| 3 AMP | 0 | Gain | 101 | 0..100 |
| 7 DLY | 1 | Time | **981** | **20..1000 ms** |
| 7 DLY | 0/2/3/4 | Mix/Fdbk/Tone/Mod | 101 each | 0..100 |

Design consequence for `CARD-BLE-202`: replace the planned
`encodeParamChange(moduleId, paramId, float)` free-form encoder with a **table-driven encoder**
that quantizes the requested float to the nearest supported discrete value. Recommended storage:
embed only the 6-byte value blocks (plus module/alg constants) and rebuild frame + CRC at runtime;
do not embed full pre-CRC'd strings. Estimated footprint: DRV+AMP gain = 202 x 6 B ~= 1.2 KB,
DLY time = 981 x 6 B ~= 5.9 KB — trivial for 16 MB flash, acceptable in rodata.

Open optimization (non-blocking): the value-block bit pattern may be computable
(e.g., monotonic nibble patterns visible in the tables). If confirmed during CARD-PRE-000,
the DLY table could shrink to an arithmetic encoder.

## 5. Tap Tempo Reference Semantics (PocketEdit)

Matches our Mode 1 design intent; adopt these exact rules:

1. Collect taps; keep last 4 timestamps; reset after 2000 ms idle.
2. BPM = 60000 / average(inter-tap intervals); requires >= 2 taps.
3. Clamp BPM to 20..300; round to nearest 0.5.
4. Delay ms = (60000 / BPM) x note multiplier; clamp to 20..1000 ms (device range).
5. Transmit `0408` (module 7, algId 1) on every recalculated value — i.e., from the
   2nd tap onward. Note-divisor buttons: 1/8, dotted 1/8, 1/4, 1/2 exist in the reference UI.

Note: SPECIFICATION.md Mode 1 says BPM range 40..300; the device supports down to 20 BPM
(1000 ms). Recommend widening the UI range to match the device.

## 6. BLE Transport Facts (verified)

* Service UUID `03b80e5a-ede8-4b33-a751-6ce34ec4c700` and characteristic
  `7772e5db-3868-4112-a1a9-f2669d106bf3` confirmed in PocketEdit source — matches
  `include/config.h` exactly.
* Advertised local name: `Sonic Master BLE`.
* Writes: write-without-response, whole frame per operation, no application-level chunking.
* USB MIDI transport exists (frame without the `80 80` header) — irrelevant to the POC
  but confirms the `80 80` bytes are a transport artifact, not protocol content.
* Pairing/bonding: PocketEdit performs no explicit pairing flow (Web Bluetooth default),
  supporting the recommendation to use Just Works / no bonding in firmware.

## 7. Open Items (to close in CARD-PRE-000)

1. Capture 3-5 golden vectors from the physical pedal (or PocketEdit live log panel) and
   freeze them as native unit-test fixtures.
2. Verify whether the pedal accepts parameter writes immediately after GATT connect,
   or requires an editor-style sync/query sequence first (PocketEdit always queries on
   connect; absence of requirement is unproven).
3. Confirm notify subscription is required for writes to be accepted (some stacks gate
   writes on CCCD configuration).
4. Determine whether the value-block encoding is arithmetically computable.
