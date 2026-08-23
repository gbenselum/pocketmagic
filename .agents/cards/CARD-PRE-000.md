# CARD-PRE-000: Protocol Evidence Pack & Golden Vector Freeze

- **Epic:** Epic 0: Preplan De-Risking (new)
- **Priority:** CRITICAL (blocks CARD-BLE-202)
- **Assigned Role:** `Agent-Comms`
- **Skill:** `comms-sysex`
- **Feature Branch:** `feature/pre-000-protocol-evidence`
- **Dependencies:** None (PocketEdit reference already located; physical pedal owned by user)
- **Token Budget:** ~12,000 Tokens

## 1. Description
Freeze the Sonicake Pocket Master protocol knowledge into this repository so that
CARD-BLE-202 can be implemented against verified facts instead of assumptions.
Source of truth: PocketEdit (`github.com/suckyble/PocketEdit`), already decoded into
`docs/PROTOCOL.md`. This card converts that documentation into machine-checkable artifacts.

## 2. Technical Requirements
1. Extract from PocketEdit's `COMMAND_LIBRARY_DATA` (script-assisted, do not hand-copy):
   - Module 2 / AlgID 0 value blocks (101 entries, DRV Gain)
   - Module 3 / AlgID 0 value blocks (101 entries, AMP Gain)
   - Module 7 / AlgID 1 value blocks (981 entries, DLY Time ms)
   - Module on/off strings for modules 0..9 (optional, small)
2. Emit as C++17 headers under `include/protocol/` using `constexpr std::array`,
   zero heap, MIT header — ready for direct consumption by CARD-BLE-202.
3. Generate golden test vectors: >= 10 full frames spanning all three parameters,
   including boundary values (gain 0, gain 100, delay 20 ms, delay 1000 ms).
4. Verify CRC scope rule (TP+PN+LEN+payload, de-expanded) against every vector;
   any mismatch invalidates `docs/PROTOCOL.md` and stops the pipeline.
5. On-hardware sanity pass (user-assisted): flash a throwaway NimBLE central sketch,
   replay two vectors against the real pedal, confirm audible parameter change.
6. Close the four open items listed at the end of `docs/PROTOCOL.md`.

## 3. Acceptance Criteria
- [ ] `include/protocol/pocket_master_tables.hpp` (+ `.cpp` if needed) committed with extracted blocks.
- [ ] Native unit-test fixtures contain frozen golden vectors; CRC recomputation matches 100%.
- [ ] At least two vectors confirmed working on physical hardware (log excerpt committed).
- [ ] Open items in `docs/PROTOCOL.md` Section 7 answered and document updated.
