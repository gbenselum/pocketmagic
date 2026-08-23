# Running the Seven Agents in Orca ADE (onorca.dev)

> Verified against Orca documentation and CLI reference on 2026-08-23.
> Harness: OpenCode. Agents live globally in `~/.config/opencode/agents/*.md`.

## 1. Why this works with zero per-project config

Orca launches agents as terminal processes. It ships an **OpenCode** entry
(auto-setup, status). Every OpenCode launch reads your global configuration, so all
seven agent definitions are available inside every Orca worktree automatically:

| Agent | Global definition |
| :--- | :--- |
| build | `~/.config/opencode/agents/build.md` |
| plan | `~/.config/opencode/agents/plan.md` |
| implement | `~/.config/opencode/agents/implement.md` |
| troubleshoot | `~/.config/opencode/agents/troubleshoot.md` |
| kubernetes-specialist | `~/.config/opencode/agents/kubernetes-specialist.md` |
| devops | `~/.config/opencode/agents/devops.md` |
| guidance-agent | `~/.config/opencode/agents/guidance-agent.md` |

Git worktrees change the checkout directory, not your home directory. The global
agent set therefore applies in every Orca worktree with no extra files.

## 2. Prerequisites (one-time)

1. Install Orca: <https://onorca.dev/download> or `brew install --cask stablyai/orca/orca`.
2. Confirm both CLIs resolve in a normal shell:
   ```sh
   opencode --version
   orca status --json
   ```
3. In Orca: Settings -> Agents -> confirm **OpenCode** is enabled.
4. Optional: register the bundled CLI via Settings -> General -> Orca CLI.

## 3. Pattern A - one pane, switch personas (simplest)

Open any worktree, start OpenCode from the agent combobox, then use the TUI's
built-in agent selector to move between the seven loaded agents. One process,
all personas reachable. Sub-agent delegation also works inside the session
(for example, kubernetes-specialist runs as a child task).

Use this when you want conversation continuity while changing roles.

## 4. Pattern B - parallel panes, one agent each

Each pane runs its own OpenCode process pinned to one persona:

```sh
opencode --agent build
opencode --agent plan
opencode --agent implement
opencode --agent troubleshoot
opencode --agent devops
opencode --agent guidance-agent
```

`kubernetes-specialist` is a subagent (`mode: subagent`); it does not take
interactive input at top level. Launch it only through delegation from another
agent session.

To make these one-click entries inside Orca, create project-scoped Quick Commands:

1. Open Settings -> Quick Commands (or the Terminal quick-command list).
2. Add one command per row above, scope: this project, name it after the agent.
3. They appear in the terminal command list and sync to the mobile companion.

Quick Commands live in Orca application storage, not in files this repository can
carry. The commands above are the complete setup; pasting them takes about two
minutes once.

## 5. Pattern C - supervised multi-agent runs (orchestration)

Orca's orchestration layer (Settings -> Experimental -> enable) supports durable
runs with tasks, dispatches, and worker reporting:

```sh
orca orchestration run-create --objective "Execute CARD-CORE-101 verification" --json
orca orchestration task-create --spec "Review platformio.ini flags against rules" --task-title "CI audit" --json
orca orchestration worker-start --task <taskId> --worktree new-child --name ci-audit --agent opencode --setup run --json
```

Known limitation, verified in Orca docs: per-worker `--model` / `--effort`
overrides apply to Claude, Codex, and Cursor only. There is no documented flag to
pin a specific OpenCode sub-agent (`--agent build`) onto a dispatched worker. Two
workarounds:

1. Put the role contract into the task spec text (the worker receives it).
2. Set Settings -> Agents -> OpenCode -> launch arguments to
   `--agent <persona>` when you want every orchestrated OpenCode worker to run as
   that single persona. This is a global override for the OpenCode entry; Orca
   treats a non-empty custom value as an explicit user override.

## 6. Worktree behavior notes

- Each Orca task gets its own git worktree branch. Commit discipline stays GitFlow:
  work happens on feature branches, merges land through `develop`.
- CI is currently disabled by request. Re-enable before merging code branches:
  ```sh
  gh api -X PUT repos/gbenselum/pocketmagic/actions/workflows/ci.yml/enable
  ```
- The `.agents/` workspace rules and cards ship inside the repository, so every
  worktree carries the same card system and STE communication rules.

## 7. Verification checklist

```sh
orca status --json                     # runtime reachable
opencode --agent guidance-agent        # opens the read-only guide persona
gh api repos/:owner/:repo --jq .default_branch   # expect: main
```

If `opencode --agent X` reports an unknown agent, list what the CLI sees with
`opencode agents` (or check the picker inside the TUI) and confirm the file exists
under `~/.config/opencode/agents/`.
