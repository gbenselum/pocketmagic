# CONVERSATION LOG

Source-of-truth log for historical setup context and configuration decisions
(per workspace convention). Newest entries at the bottom.

---

## 2026-08-23 — Preplan Feasibility Review & Documentation Baseline

**Session scope:** Full repository review, external evidence gathering, preplan docs.
No implementation performed (user directive).

### Work completed
1. Read 100% of repository files: SPECIFICATION.md, AGENTS.md, platformio.ini,
   .gitignore, include stubs, all 10 cards + INDEX, all rules, all skills,
   antigravity chart, ci.yml. Verified `src/`, `sim/`, `test/` empty (pre-implementation).
2. Feasibility review produced with findings R1-R13 — see `docs/FEASIBILITY_REVIEW.md`.
3. External verification:
   - Wokwi officially does NOT simulate Bluetooth on any chip; NimBLE init hangs the sim;
     no M5Stack Core2 board model exists. Wokwi pillar of CARD-SIM-501 rescoped (A-05/A-06).
   - Python BLE peripheral emulation infeasible on macOS dev machine -> mock-pedal ESP32
     sketch adopted as primary emulator proposal.
   - CI defects identified: release job missing `permissions: contents: write` (would 403);
     native test failures swallowed by `|| echo` (A-08/A-09).
4. **Protocol breakthrough:** located and analyzed PocketEdit
   (`github.com/suckyble/PocketEdit`) — reverse-engineered Sonicake Pocket Master editor.
   Extracted and verified:
   - Correct frame layout incl. TP/PN/LEN header fields that SPECIFICATION.md 3.2 omitted.
   - CRC-8/SMBus input scope = TP+PN+LEN+payload de-expanded (spec text was wrong;
     the C algorithm snippet itself is correct).
   - Parameter values are table-driven proprietary encodings, not integers:
     DRV gain 101 vals, AMP gain 101 vals, DLY time 981 vals spanning 20..1000 ms.
   - Tap tempo reference semantics (20..300 BPM clamp, last-4-tap average, 2 s reset).
   - GATT UUIDs match our config.h character-for-character.
   - Whole-frame single write-without-response; worst POC frame ~50 bytes.
   - Distilled into `docs/PROTOCOL.md`; CARD-PRE-000 created as blocking prerequisite
     for CARD-BLE-202.

### User decisions recorded
| Decision | Choice |
| :--- | :--- |
| Framework | Arduino via PlatformIO stays (ease/compatibility priority) |
| Hardware | Purchase deferred until after plan lock; single device preferred |
| Second board | Optional only; documented in `requiredhardware.md` (mock-pedal use case) |
| PocketEdit | Adopted as protocol source of truth |
| Scope guard | Preplan only — markdown plans/suggestions committed; zero implementation |

### Repository changes (this commit)
- Added: `docs/FEASIBILITY_REVIEW.md`, `docs/PROTOCOL.md`, `docs/SPEC_AMENDMENTS.md`,
  `requiredhardware.md`, `.agents/cards/CARD-PRE-000.md`, this log.
- Modified: `.agents/cards/INDEX.md` (PRE-000 row, budget 211k -> 223k, execution order).
- Git topology: created `main` from spec-branch tip and pushed; created `develop` at same
  tip for future card work (amendment A-13 partially executed).

### Open items / next actions
1. User reviews `docs/SPEC_AMENDMENTS.md` A-01..A-16 and approves/rejects each.
2. On approval: apply amendments inside their owning cards (no doc-only pass needed
   except SPECIFICATION.md edits, which should be one dedicated docs commit).
3. GitHub default branch switch to `main` + branch protection (needs repo admin UI or gh CLI).
4. After hardware purchase: CARD-PRE-000 hardware sanity step, then INDEX execution order.
