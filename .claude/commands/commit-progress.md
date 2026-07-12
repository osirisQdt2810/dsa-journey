---
description: Commit study progress — daily LeetCode (main repo) + course lessons/homework/revision (submodules), one commit per unit, on the correct per-repo branch
argument-hint: "[optional: paths/names to restrict scope, e.g. 2026_07_06 | lesson-06-graph | 01-two-pointers-sliding-window; else all pending]"
allowed-tools: Bash(git:*), Bash(ls:*), Read
---

Commit uncommitted study work across **four targets in three repos** (the main repo hosts both Target A and Target D), each with its own scope, branch convention, and commit-message format. This supersedes the old `commit-daily-leetcode` command and fully covers it (the daily-LeetCode case is Target A below).

The two `courses/*` folders are **git submodules** — commit *inside* the submodule with `git -C courses/<name> …`, on that submodule's own branch. They are independent repos.

## Targets

| # | Repo | Scope (only these paths) | Unit = one commit |
|---|------|--------------------------|-------------------|
| A | `.` (main repo `dsa-journey`) | `src/leetcode/daily/YYYY_MM_DD/` | one **date folder** |
| B | `courses/dsa-mentorship` (submodule) | `lesson-NN-<topic>/` and `lesson-NN-<topic>/hw/` | one **problem** (its `X. *.cpp` + matching `input_X.txt`) |
| C | `courses/fse13-faang` (submodule) | `NN-<topic>/{homework,livecoding,revision}/` | one **problem** file (+ any sibling input) |
| D | `.` (main repo `dsa-journey`) | `src/leetcode/problems/<topic>/` | one **problem** file `NNNN. *.cpp` (+ any sibling input) — a self-study *review series* |

- If `$ARGUMENTS` names specific date folders / lesson folders / topic folders, restrict to those; otherwise commit **every** pending unit found across all three targets.
- Discover pending work per repo with:
  - `git status --porcelain src/leetcode/daily/` (Target A)
  - `git status --porcelain src/leetcode/problems/` (Target D)
  - `git -C courses/dsa-mentorship status --porcelain`
  - `git -C courses/fse13-faang status --porcelain`
- **Never** run `git add -A`. Stage only the exact files of the unit being committed. Leave everything out of scope untouched (other submodules, `build/`, slides, READMEs, etc.).

## Branch handling — do this BEFORE committing each unit (critical)

Each repo must be on the **correct feature branch** for the work. Never commit on `master` / `main` / a default branch.

1. Derive the expected branch from the folder being committed:
   - **Target A (daily):** `phuc-nguyen/daily-lc-challenge`
   - **Target B (dsa lesson `lesson-NN-<topic>`):** `phuc-nguyen/lesson-NN-<topic>` (e.g. `lesson-06-graph/` → `phuc-nguyen/lesson-06-graph`)
   - **Target C (fse13 folder `NN-<topic>`):** `phuc-nguyen/review-NN-<topic>` — but **first prefer an existing local branch matching `phuc-nguyen/review-NN*`** (`git -C courses/fse13-faang branch --list 'phuc-nguyen/review-NN*'`), because topics are sometimes shortened (folder `01-two-pointers-sliding-window` → branch `phuc-nguyen/review-01-two-pointers`). Use the existing match if there's exactly one; if none, create `phuc-nguyen/review-NN-<full-topic>` and flag the name in your summary.
   - **Target D (main-repo `src/leetcode/problems/<topic>/`):** `phuc-nguyen/review-<series>` — a self-defined *review series* that groups several related `problems/<topic>` folders under one branch (e.g. `phuc-nguyen/review-graph-sp-mst` covers `shortest_path/` + MST; `phuc-nguyen/review-two-pointer` covers `two_pointers/`). The folder name does **not** map 1:1 to a branch. Resolve the branch like this:
     1. Determine the problem's **canonical topic** from its `problems/<topic>/` folder (and the code). The full topic vocabulary lives in `scripts/find_unsolved_leetcode.py` → `TOPIC_ALIASES` (e.g. folder `shortest_path` → `shortest-path`, `two_pointers` → `two-pointers`, `dsu` → `union-find`).
     2. **Prefer an existing `phuc-nguyen/review-*` branch whose series covers that topic** (`git branch --list 'phuc-nguyen/review-*'`). If the **current** branch is already that review branch, stay on it. Use an existing match when exactly one series fits (e.g. `shortest-path`/`mst` → `review-graph-sp-mst`).
     3. If **no** review branch covers the topic, **create** `phuc-nguyen/review-<series>`, naming `<series>` from the topic slug(s) in the script's `TOPIC_ALIASES` (a short family name is fine, e.g. `review-trie`, `review-segtree`). Flag the created name. Only **ask** the user when it's genuinely ambiguous which existing series the problem belongs to.
     - These review branches often sit **behind `master`** (an already-merged series) — commit the new problem on the branch as-is and note it's behind master; do **not** rebase/rewrite without asking.
2. Check the repo's current branch (`git -C <repo> rev-parse --abbrev-ref HEAD`).
   - Already on the expected branch → proceed.
   - Expected branch exists locally → `git -C <repo> checkout <branch>` (new practice files are untracked and carry over safely).
   - Expected branch does **not** exist → create it. If the scoped changes are **all untracked** (new files), branch off the repo's default branch for a clean topic branch: `git -C <repo> checkout <main|master>` then `git -C <repo> checkout -b <branch>` (untracked files survive the switch). If any scoped file is a **modified tracked** file (so switching base could conflict), instead create the branch off the current HEAD (`git -C <repo> checkout -b <branch>`) and flag it. Report which base was used.
   - If any `checkout` fails (conflict / dirty tree), **stop and report** — do not force anything.
3. When multiple units map to different branches (e.g. two lesson folders), process them **one branch-group at a time**: checkout that branch, commit all its units, then move to the next.

## Commit-message formats

Pick `[add]` for brand-new files (untracked / added), `[update]` for modifications to already-tracked files (derive from the porcelain status code).

**Target A — daily LeetCode** (one commit per date folder; `YYYY_MM_DD` → `DD-MM-YYYY`):
```
[daily] add leetcode daily challenge DD-MM-YYYY (lc <NUMBER> - <technique>)
```

**Target B — dsa-mentorship lesson problem** (`<LETTER>` from the file `A. ….cpp` → `A`):
```
[add][lesson-NN-<topic>] <LETTER>: <technique>
```

**Target B — dsa-mentorship homework** (files under `lesson-NN-<topic>/hw/`; the tag is literally `[homework]` even though the folder is `hw`):
```
[add][lesson-NN-<topic>][homework] <LETTER>: <technique>
```

**Target C — fse13-faang** (keep the style already in that repo's git log — `[add]`/`[update]` + free-text + `(lc …)`):
```
[add] <technique> (lc <NUMBER>)
```
- `<technique>` here is a concise **5-10 word** main-technique phrase (same spirit as the other repos — not a long free-text sentence).
- LC number from the filename's leading 4-digit id (keep zero-padding, e.g. `lc 0076`). If it's a homework-numbered file (`1. ….py`, no LC id), use `(hw <n>)` instead of `(lc …)`.

**Target D — main-repo review problem** (`src/leetcode/problems/<topic>/`; keep the free-text style already in this repo's git log for `problems/`):
```
[add] solve <Problem Name> (lc <NUMBER>) - <technique>
```
- `[update]` for modifications to already-tracked files.
- `<Problem Name>` = the LeetCode title (from the filename, minus the leading id). `<NUMBER>` = the code's real LC id (strip zero-padding, e.g. `lc 3604`).
- `<technique>` here is a **descriptive** phrase/clause (longer than the other targets — a short sentence is fine), matching the existing `problems/` commits, e.g. `Dijkstra with time-windowed edges, arrival = max(t, start)+1`.

`<technique>` = the **core idea**: max **5-6 words** (Targets A/B), **5-10 words** (Target C), or a **descriptive phrase/clause** (Target D).

## How to fill in NUMBER and technique
- **Read the source file(s)** in each unit first. Derive both the problem number and the technique from the **actual code** (the solved function + algorithm), NOT from the filename — filenames are often leftover templates and may not match the code inside.
- If the filename's number disagrees with the code, keep the filename as-is but use the **code's** real number in the message, and note the mismatch in your summary.

## Guardrails
- **Do NOT add any `Co-Authored-By` / trailer line.** The user writes all the solution code; commits are authored solely by the user.
- **Never** `git add -A`; stage only the unit's own files.
- If a unit's solution is **empty/unsolved** or the code is broken, do NOT commit it silently — flag it and ask whether to skip, or wait for the user to finish it.
- If a unit is genuinely ambiguous (can't tell the problem/technique from the code), ask rather than guess.
- Committing inside a submodule leaves the **parent repo's submodule pointer** showing as modified — that's expected. Do **not** auto-commit the pointer bump in the parent; leave it for the user.
- Do **not** push. Only push (or force-push) if the user explicitly asks.

## After committing
For each repo you committed in, print its `git log --oneline` for the new commits and the branch it's on, then a combined table of `repo | branch | unit | number | technique`, plus any flagged/skipped units and any branches you created (with their base).

Arguments: $ARGUMENTS
