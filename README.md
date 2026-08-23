# PocketMagic

ESP32-based headstock motion and touchscreen controller for the Sonicake
Pocket Master multi-effects pedal. M5Stack Core2 hardware, BLE MIDI central,
plugin architecture. See `SPECIFICATION.md` for the full design and
`docs/FEASIBILITY_REVIEW.md` for the current project status.

## Status

Pre-plan phase: specification validated, protocol reverse-engineered and
documented in `docs/PROTOCOL.md`, hardware list in `requiredhardware.md`.
Implementation starts with `CARD-CORE-101` after amendment approval.

## Agent Workflow

Multi-agent development under `.agents/` (cards, rules, skills, token ledgers).
Token accounting protocol: see the Token Accounting Rules section of `AGENTS.md`.

- Per-agent ledgers: `.agents/agent_<name>_consumed_tokens.md`
- Unified total: `total_tokensconsumed.md`
- Every PR states its consumed tokens; every merge to main regenerates the total.

---

This application was made with 0 tokens
