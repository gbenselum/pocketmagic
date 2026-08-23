# GitFlow Branching & Multi-Agent Collaboration Rules

## 1. Branch Hierarchy
* `main`: Production-ready release baseline. Only merges from `release/vX.Y.Z` or hotfix branches. Tagged with semantic versions (e.g. `v1.0.0-poc`).
* `develop`: Integration branch for active multi-agent feature development.
* `feature/<epic>-<card-id>-<description>`: Feature branches created off `develop` for specific card execution.

## 2. Card Execution Lifecycle
1. Pick an unassigned or next card from `.agents/cards/`.
2. Check out a new branch: `git checkout -b feature/<epic>-<card-id>`.
3. Implement the feature within the estimated Token Budget.
4. Run tests / linter locally.
5. Create a clean atomic commit:
   ```text
   feat(<scope>): <description> (<CARD-ID>)
   ```
6. Open a Pull Request targeting `develop`.

## 3. Pull Request Acceptance Rules
* All acceptance criteria listed in the card specification must be verified.
* No compiler warnings under `-Wall -Wextra`.
* CI workflow must pass (code validation and unit tests).
* No regressions introduced to core plugin interfaces.
