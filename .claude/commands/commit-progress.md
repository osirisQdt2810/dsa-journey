---
description: Commit study progress — daily LeetCode + review problems (main repo) and course lessons/homework/revision (submodules), one commit per unit, on the correct per-repo branch; `push` also rebases+pushes every touched branch, `pr` additionally opens PRs, waits for auto-merge and rebase-sweeps the other local branches
argument-hint: "[optional mode: push | pr — omit for local commits only] [optional: paths/names to restrict scope, e.g. 2026_07_06 | lesson-06-graph | 01-two-pointers-sliding-window; else all pending]"
model: sonnet
allowed-tools: Bash(git:*), Bash(ls:*), Bash(gh:*), Bash(pre-commit:*), Bash(g++:*), Bash(python3:*), Read, Edit, WebFetch
---

Commit uncommitted study work across **four targets in three repos**, each with its own scope,
branch convention and commit-message format. The two `courses/*` folders are **git submodules** —
independent repos: commit *inside* them with `git -C courses/<name> …`, on their own branches.

If the first token of `$ARGUMENTS` is `pull-lc` or `test-lc`, stop: tell the user these are now
`/leetcode pull <ID>` and `/leetcode test <ID>` (`/leetcode pull test <ID>` does both).

## Targets

| # | Repo | Scope (only these paths) | Unit = one commit |
|---|------|--------------------------|-------------------|
| A | `.` (main repo `dsa-journey`) | `src/leetcode/daily/YYYY_MM_DD/` | one **date folder** |
| B | `courses/dsa-mentorship` | `lesson-NN-<topic>/` and `lesson-NN-<topic>/hw/` | one **problem** (its `X. *.cpp` + matching `input_X.txt`) |
| C | `courses/fse13-faang` | `NN-<topic>/{homework,livecoding,revision}/` | one **problem** file (+ any sibling input) |
| D | `.` (main repo) | `src/leetcode/problems/<topic>/` | one **problem** file `NNNN. *.cpp` (+ any sibling input) — a self-study *review series* |

- Strip the publish mode (`push` / `pr`, see **Publish**) from `$ARGUMENTS`; remaining tokens are
  scope filters (date / lesson / topic folders). No filter → every pending unit in all three repos.
- Discover pending work: `git status --porcelain src/leetcode/daily/` (A),
  `git status --porcelain src/leetcode/problems/` (D), `git -C courses/dsa-mentorship status
  --porcelain` (B), `git -C courses/fse13-faang status --porcelain` (C).
- **Never** `git add -A`. Stage only the exact files of the unit. Leave everything out of scope
  untouched (other submodules, `build/`, slides, READMEs…) — the only exception is the README stats
  refresh inside each main-repo PR (**Publish**).

## Branch handling — BEFORE committing each unit

Never commit on `master` / `main`.

1. Expected branch per folder:
   - **A:** `phuc-nguyen/daily-lc-challenge`
   - **B** `lesson-NN-<topic>`: `phuc-nguyen/lesson-NN-<topic>`
   - **C** `NN-<topic>`: prefer the one existing local `phuc-nguyen/review-NN*` branch (topics are
     sometimes shortened: `01-two-pointers-sliding-window` → `phuc-nguyen/review-01-two-pointers`);
     if none, create `phuc-nguyen/review-NN-<full-topic>` and flag the name.
   - **D** `problems/<topic>/`: `phuc-nguyen/review-<series>`, a self-defined series grouping several
     related folders (e.g. `review-graph-sp-mst` covers `shortest_path/` + MST). Take the problem's
     canonical topic from its folder and code (vocabulary: `scripts/leetcode.py` → `TOPIC_ALIASES`;
     `shortest_path` → `shortest-path`, `dsu` → `union-find`), prefer the one existing
     `phuc-nguyen/review-*` whose series covers it, else create `phuc-nguyen/review-<series>` from the
     topic slug(s) and flag it. Ask only when it is genuinely ambiguous which series fits.
     - The folder decides the branch, **never** whatever is checked out. Re-derive it before staging
       each D unit and compare with `git rev-parse --abbrev-ref HEAD`; switch if they differ. Units
       from folders resolving to different series go on different branches.
     - Review branches often sit behind `master`; commit on them as-is and note it. Only `push`/`pr`
       rebase them.
2. On the expected branch → proceed. It exists → `git -C <repo> checkout <branch>` (untracked files
   carry over). It does not exist → if all scoped changes are untracked, create it off the
   up-to-date base (`fetch`, `checkout <base>`, `merge --ff-only <remote>/<base>`,
   `checkout -b <branch>`); if a scoped file is a modified tracked file, create it off the current
   HEAD and flag it. Report the base used. A failing checkout → stop and report, never force.
3. Process one branch-group at a time: check out, commit all its units, move on.

## Commit messages

`[add]` for new (untracked) files, `[update]` for modified tracked files (from the porcelain code).

| Target | Format | `<technique>` |
|---|---|---|
| A | `[daily] add leetcode daily challenge DD-MM-YYYY (lc <NUMBER> - <technique>)` | ≤ 5-6 words |
| B lesson | `[add][lesson-NN-<topic>] <LETTER>: <technique>` (`A. ….cpp` → `A`) | ≤ 5-6 words |
| B homework (`hw/`) | `[add][lesson-NN-<topic>][homework] <LETTER>: <technique>` | ≤ 5-6 words |
| C | `[add] <technique> (lc <NUMBER>)` — keep zero-padding (`lc 0076`); homework-numbered files use `(hw <n>)` | 5-10 words |
| D | `[add][review][<topic>] solve lc <NUMBER> - <technique>` | descriptive clause |

- D: `<topic>` is the canonical slug from `TOPIC_ALIASES` (not the folder or series name);
  `<NUMBER>` has no zero-padding. Folder `problems/ad_hoc/` → slug `ad-hoc`, branch
  `phuc-nguyen/review-ad-hoc` — only for problems solved by direct reasoning with no characteristic
  technique (unclassified problems go to `problems/unknown/`). Example:
  `[add][review][shortest-path] solve lc 3112 - Dijkstra on undirected graph, skip node v when arrival >= disappear[v]`
- Take NUMBER and technique from the **code** (the solved method + algorithm), not the filename. If
  they disagree, keep the filename, use the code's number, and note the mismatch.

## Lint before staging (main repo, REQUIRED)

pre-commit (`.pre-commit-config.yaml`: clang-format, `g++ -std=c++20 -fsyntax-only`, whitespace/EOF)
also runs in CI on every PR, so unlinted commits block auto-merge.

- After annotating each A/D unit: `pre-commit run --files <unit files>`, re-run until clean (fixers
  edit in place and fail the first pass), stage the result. Lint fixes belong to the unit commit.
- A syntax failure on a **missing `#include`** → add the specific header (never `<bits/stdc++.h>`,
  absent on Apple clang) and note it. Repo-relative includes resolve via `-I.`, so a failure there is
  a wrong path. Any **other** syntax failure = broken code → guardrail (flag, don't commit).
- **`R100` moves only:** commit the rename alone with `--no-verify` and a `[chore][review][<topic>]`
  message saying the content is unchanged; flag that those files stay unformatted (CI lint would fail
  if published — reformat in a dedicated commit after asking).
- Style hooks exclude archived areas (`daily/2024_*`, `daily/2025_*`, `contests/`, `cses/`, `vnoi/`,
  `codeforces/`); everything else, incl. `daily/2026_*` and `problems/`, is in scope and its tidy-up
  goes in the unit commit. Never extend the exclude list to dodge a failure.
- Submodules have no pre-commit config: commit as-is. Missing `pre-commit` → `pipx install pre-commit`.

## Annotate each solution (REQUIRED)

Before staging, the top of each solution file (after includes, before `Solution`) holds **three
block comments** (`/* … */`; `""" … """` in Python), all in the same unit commit (if a file was
committed this run without them, `--amend`).

**Block 1 — the problem**, from the source, never from memory:
- **LeetCode (A, C, D):** the block `python3 scripts/leetcode.py pull <N>` writes (`--dry-run` to
  preview): `LC <N> - <Title>`, the URL, the verbatim statement, examples and constraints. Keep a
  pulled block **exactly as pulled**; write blocks 2-3 below it. Pulled for the wrong problem
  (filename id ≠ code) → re-pull with the code's id via `--file`.
- **Codeforces (B):** use the URL in the file's leading `//` comment (WebFetch): title + URL, the
  statement with constraints, 2-3 representative examples, concise but never reworded numbers.
- C homework without an LC id: the URL the file references.
- Fetch fails (offline, 403, login wall, no URL) → write it from knowledge, mark it
  `(statement not verified against source — reconstructed)`, and flag the unit in the summary.

**Attempt metadata (LeetCode units)** — lines right under the URL, read by the feed:
```
    @status: self                 // self | hint | editorial | copied
    @time: 35                     // minutes from @created to the first Accepted after it
    @wa: 2                        // rejected submissions between @created and that Accepted
    @created: 2026-11-13 21:05    // when the solution file was created (local time)
```
`self` = alone; `hint` = needed a hint; `editorial` = read it, then implemented; `copied` = not the
user's code (a "debt" until re-solved alone ≥ 7 days later).
- For every **new** file with no `@status`, at the **start of the run, before any staging, commit or
  branch switch** (a commit + checkout resets the birth time), run
  `python3 scripts/leetcode.py attempt <file>...`. It measures `@created` (existing line, else the
  untracked file's birth time), the first **Accepted at/after `@created`** (older ones are an earlier
  solve, ignored; via `scripts/.session`; none → the file's last edit), `@time` = that − `@created`
  in minutes (multi-day is fine) and `@wa` = rejected submissions in between (`?` without a valid
  session or an Accepted).
- Ask the user **once** for all such units: the `@status`, showing measured `@time`/`@wa` as defaults
  (e.g. `1524: status? (measured: time 2217 min, wa 2)`); ask a value only where the script printed
  `?`. Relay its notes (expired session → suggest `python3 scripts/leetcode.py login`).
- Record with `python3 scripts/leetcode.py attempt <file> --write --status <s> [--time <min>] [--wa <n>]`.
  Skipped unit → write nothing and report it. Never invent a value. Existing lines stay as written.

**Block 2 — `Best solution`** (the objectively optimal algorithm, independent of the code):
`Best solution: <name>`; `Complexity:` with `Time:` / `Space:` in Big-O and a one-clause
justification each; `Approach (detail):` numbered steps explaining how and why it works (the key
insight), enough to reconstruct it. If you cannot confidently determine it, ask.

**Block 3 — `Your solution`** (what the committed code actually runs):
`Your solution: <name>` (several implementations → the live one, plus
`Also in the file (not called): …`); `Complexity:` of the **code**; `Approach:` numbered walk-through
using the code's own names; `Verdict:` exactly one of
- `MATCHES BEST` — implements the documented optimum;
- `SUBOPTIMAL` — correct but worse: name the gap (`O(n^2) vs O(n log C)`) and whether it passes;
- `INCORRECT` — name the failing case;

plus how it was verified when tested. Never `MATCHES BEST` for a non-optimal live method. Any other
verdict is repeated in the summary. A file with the older single-block analysis is split into blocks
2/3 in its unit commit.

```
/*
    LC 856 - Score of Parentheses
    https://leetcode.com/problems/score-of-parentheses/
    ...statement, examples, constraints...
*/
/*
    Best solution: depth counting

    Complexity:
        Time:  O(n) — one scan.
        Space: O(1) — a depth counter and the sum.

    Approach (detail):
        1. ...
*/
/*
    Your solution: stack of '(' indices propagating doubled inner scores

    Complexity:
        Time:  O(n) — each character is pushed / popped once.
        Space: O(n) — the stack and score[].

    Approach:
        1. ...

    Verdict: SUBOPTIMAL — same O(n) time but O(n) vs O(1) extra space; irrelevant at n <= 50.
        Verified on all balanced strings up to length 18.
*/
```

## Guardrails
- **No `Co-Authored-By` / trailer lines** — the user writes all the solution code.
- Empty / unsolved / broken solution → do not commit it silently; flag it and ask (skip or wait).
- Genuinely ambiguous unit (problem or technique unclear from the code) → ask.
- Submodule commits leave the parent's submodule pointer modified — leave that bump to the user, in
  every mode.
- Nothing is pushed without a mode word. Force-push only with `--force-with-lease`, only on a branch
  you just rebased, never on a base branch or one whose upstream is the base (`cheatsheet` tracks
  `origin/master`). Never `--force`, never push to `master`/`main`, never merge a PR yourself — the
  only merge path is the `auto-merge` label + the CI bot.

## Publish — `push` / `pr`

One bare mode word (case-insensitive; both given → `pr`, say so). Publishing runs **after** all local
commits.

| mode | branch set | does |
|---|---|---|
| *(none)* | — | local commits only; say nothing was pushed and name the mode words |
| `push` | branches that received a commit this run, all repos | Step 1 only |
| `pr` | **every** local `phuc-nguyen/*` branch with `git -C <repo> rev-list --count <base>..<branch>` > 0, committed this run or not — compute it explicitly | Steps 1-5 |

Base and remote per repo — derive the remote from `git -C <repo> rev-parse --abbrev-ref
<base>@{upstream}` (never hardcode `origin`): main repo `master`/`origin`, `courses/dsa-mentorship`
`main`/`origin`, `courses/fse13-faang` `main`/**`fseorg`**.

**Step 1 — rebase + push**, per repo, then per branch:
1. `fetch <remote>`, `checkout <base>`, `merge --ff-only <remote>/<base>`; ff fails → base diverged:
   publish nothing in that repo, report it.
2. `checkout <branch>`, `rebase <base>`; conflict → `rebase --abort`, report *not published*, go on.
3. No upstream → `push -u <remote> <branch>`; else `push --force-with-lease <remote> <branch>`.
   Rejected lease → report, never retry with `--force`. `Everything up-to-date` is a success and the
   branch stays in scope for Step 2.

**Main-repo branches go one at a time** through Steps 5 → 2 → 3 (two open PRs rewriting the same
README lines would conflict): refresh README → push → PR → wait → ff `master` → rebase the next
branch onto it.

**Step 5 — README refresh, the last commit of each main-repo PR** (never a separate PR; unit commits
and `push` never touch the README):
- Branch tip is an older `[docs] refresh README stats` commit not in `master` → `git reset --keep
  HEAD~1` before rebasing.
- After the rebase: `python3 scripts/update_readme_stats.py --check --ref HEAD`. Up to date and no
  `warning:` → no README commit (say so). Otherwise run it without `--check`, add each warned entry
  (missing `topics/**/*.hpp` header, missing `scripts/*.py` in Tooling) in the README's style, re-check
  until clean, `pre-commit run --files README.md`, commit only `README.md` as
  `[docs] refresh README stats (<N> solutions, <M> daily folders)`, push.

**Step 2 — PRs, for every branch in the set** (also those that were up to date):
- Main repo: `gh label create auto-merge --color 0E8A16 --description "CI merges this PR when lint
  passes and there are no conflicts" 2>/dev/null || true`; then `gh pr list --head <branch> --state
  open` — none → `gh pr create --base master --head <branch> --fill --label auto-merge`; open →
  `gh pr edit <n> --add-label auto-merge`. A *merged* PR from an earlier run does not count.
- Submodules: `gh pr create -R <owner>/<repo> --base main --head <branch> --fill`, **no** label (no
  CI); report it waits for a manual merge.

**Step 3 — wait (main repo only):** poll `gh pr view <n> --json state,mergedAt,mergeStateStatus` about
every 20 s, ≤ ~10 min total. `MERGED` → base advanced. Still open, closed unmerged, or a conflict
comment → *published but not merged*, move on. Never wait on submodule PRs.

**Step 4 — rebase sweep, only if a base actually advanced** (re-fetch + ff; unchanged → skip and say
so). For each local `phuc-nguyen/*` branch (not the base, not one tracking the base): ahead 0 →
`merge --ff-only <base>` and plain push if it has an upstream; else `rebase <base>` and
`--force-with-lease` (conflict → abort, leave it, report). No upstream → rebase locally, never push.
A branch checked out in another worktree is skipped and reported. Submodules: only if their base moved.

## Report
- Per repo: the new commits (`git log --oneline`) and their branch; a table
  `repo | branch | unit | number | technique`; flagged/skipped units; created branches + base.
- `push`/`pr`: `repo | branch | pushed? | how`, with the reason for anything not pushed.
- `pr`: `PR URL | merged?` (submodule PRs: *manual merge, no CI*); a sweep table
  `repo | branch | rebased or ff | re-pushed? | conflict?` or "sweep skipped: base never moved"; the
  README refresh of each main-repo PR (old → new numbers, or "already up to date").
- **Every run — stranded work:** each local `phuc-nguyen/*` branch with ahead > 0 and no open PR,
  reported as *unmerged work with no PR* with its commit count.
- **Every run, last — cultivation feed:** `python3 scripts/leetcode.py feed` (reads all of
  `src/leetcode/`, harmless with nothing committed, writes `~/.omnia/cultivation/dsa_feed.json` or
  `$OMNIA_CULTIVATION_FEED`). Put its one-line summary and warnings in the report; a failure never
  fails the command.

Arguments: $ARGUMENTS
