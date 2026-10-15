---
description: Commit study progress — daily LeetCode (main repo) + course lessons/homework/revision (submodules), one commit per unit, on the correct per-repo branch; the `push` mode also rebases+pushes every touched branch, and `pr` additionally opens PRs, waits for auto-merge, then rebase-sweeps the other local branches; `pull-lc <ID>` instead pulls a LeetCode statement verbatim into its solution file (no commit)
argument-hint: "[pull-lc <ID>... — only pull LeetCode statements into their files, nothing else] | [optional mode: push | pr — push rebases+pushes every touched branch, pr also opens PRs and waits for auto-merge then rebase-sweeps the rest; omit for local commits only] [optional: paths/names to restrict scope, e.g. 2026_07_06 | lesson-06-graph | 01-two-pointers-sliding-window; else all pending]"
allowed-tools: Bash(git:*), Bash(ls:*), Bash(gh:*), Bash(pre-commit:*), Bash(g++:*), Bash(python3:*), Read, Edit, WebFetch
---

Commit uncommitted study work across **four targets in three repos** (the main repo hosts both Target A and Target D), each with its own scope, branch convention, and commit-message format. This supersedes the old `commit-daily-leetcode` command and fully covers it (the daily-LeetCode case is Target A below).

The two `courses/*` folders are **git submodules** — commit *inside* the submodule with `git -C courses/<name> …`, on that submodule's own branch. They are independent repos.

## Subcommand — `pull-lc <ID>...` (statement only, no commit)

When the **first** token of `$ARGUMENTS` is `pull-lc`, run **only** this and skip everything else in
this file (no discovery, no branch switching, no lint, no commit, no push). Every following token is
a LeetCode problem number; optionally a token after an id may be a file path to pin the target file.

For each id run:
```
python3 scripts/pull_leetcode_statement.py <ID> [--file <path>]
```
The script resolves the id → slug (`https://leetcode.com/api/problems/all/`), fetches the statement
HTML from LeetCode's GraphQL `question` query, and writes it into the solution file as a comment
block **verbatim** — every sentence, every example, the constraints and any follow-up, with
`<sup>`/`<sub>` rendered as `^`/`_`. **Images are dropped; nothing else is.** Never edit, trim or
reword the pulled text by hand.

- **Target file:** the single file under `src/leetcode/` or `courses/` named `<ID>. *` (zero-padding
  allowed). If several match (e.g. a daily copy and a `problems/` copy) or none does, the script
  stops and lists them — ask the user which one, then re-run with `--file`.
- **Placement:** an earlier pulled block for the same id is replaced in place (re-running is
  idempotent); otherwise the first empty `/* */` placeholder (the daily template has one above
  `class Solution`) is filled; otherwise a new block goes right after `using namespace std;`.
- The block's first lines are `LC <ID> - <Title>` and the problem URL.
- The script warns when LeetCode's C++ method name does not appear in the file — that means the
  filename's id names a different problem than the code solves. Relay the warning; do not "fix" it.
- If the fetch fails (offline, premium-only), report it and stop — never write a statement from
  memory in this subcommand.
- Report per id: the file touched and how (filled placeholder / replaced / inserted). The file stays
  uncommitted; it is committed later with its solution by a normal `/commit-progress` run.

## Targets

| # | Repo | Scope (only these paths) | Unit = one commit |
|---|------|--------------------------|-------------------|
| A | `.` (main repo `dsa-journey`) | `src/leetcode/daily/YYYY_MM_DD/` | one **date folder** |
| B | `courses/dsa-mentorship` (submodule) | `lesson-NN-<topic>/` and `lesson-NN-<topic>/hw/` | one **problem** (its `X. *.cpp` + matching `input_X.txt`) |
| C | `courses/fse13-faang` (submodule) | `NN-<topic>/{homework,livecoding,revision}/` | one **problem** file (+ any sibling input) |
| D | `.` (main repo `dsa-journey`) | `src/leetcode/problems/<topic>/` | one **problem** file `NNNN. *.cpp` (+ any sibling input) — a self-study *review series* |

- `$ARGUMENTS` may contain a publish mode — the bare word `push` or `pr` (see **Publish modes** below) — strip it first; the remaining tokens are scope filters.
- If the remaining `$ARGUMENTS` name specific date folders / lesson folders / topic folders, restrict to those; otherwise commit **every** pending unit found across all three targets.
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
     2. **Prefer an existing `phuc-nguyen/review-*` branch whose series covers that topic** (`git branch --list 'phuc-nguyen/review-*'`). Use an existing match when exactly one series fits (e.g. `shortest-path`/`mst` → `review-graph-sp-mst`).
     3. If **no** review branch covers the topic, **create** `phuc-nguyen/review-<series>`, naming `<series>` from the topic slug(s) in the script's `TOPIC_ALIASES` (a short family name is fine, e.g. `review-trie`, `review-segtree`). Flag the created name. Only **ask** the user when it's genuinely ambiguous which existing series the problem belongs to.
     - **The branch is decided by the problem's `problems/<topic>/` folder, NEVER by whichever branch happens to be checked out.** The current branch counts only when it is *independently* the branch the folder resolves to. A new file in `problems/ad_hoc/` while HEAD sits on `phuc-nguyen/review-graph-sp-mst` must be committed on the ad-hoc series branch (creating it if needed) — **not** on `review-graph-sp-mst` just because you were already there. Before staging any Target D unit, re-derive the branch from its folder and compare it against `git rev-parse --abbrev-ref HEAD`; if they differ, check out (or create) the right one first.
     - Units from **different `problems/<topic>` folders that resolve to different series must go on different branches**, each in its own checkout — even when they were all written in the same sitting and show up in one `git status`. Never let convenience batch unrelated topics onto one branch.
     - These review branches often sit **behind `master`** (an already-merged series) — commit the new problem on the branch as-is and note it's behind master; do **not** rebase/rewrite without asking. Exception: the `push` / `pr` mode authorizes exactly this rebase, as part of the publish flow (see **Publish modes**).
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

**Target D — main-repo review problem** (`src/leetcode/problems/<topic>/`):
```
[add][review][<topic>] solve lc <NUMBER> - <technique>
```
- `[update]` (instead of `[add]`) for modifications to already-tracked files.
- `[<topic>]` = the problem's **canonical topic slug** from `scripts/find_unsolved_leetcode.py` → `TOPIC_ALIASES` (e.g. `shortest-path`, `two-pointers`, `union-find`) — **not** the `problems/<topic>/` folder name and **not** the branch/series name.
  - Exception: folder `problems/ad_hoc/` → slug `ad-hoc`, series branch `phuc-nguyen/review-ad-hoc`. This one is deliberately absent from `TOPIC_ALIASES` because that table's values are real LeetCode tag slugs and LeetCode has no tag for "needs no specific technique". Use `ad-hoc` only for problems solved by direct reasoning/implementation with no characteristic algorithm or data structure — never as a dumping ground for problems you have not classified yet (that is what `problems/unknown/` is for).
- `<NUMBER>` = the code's real LC id, **without** zero-padding (e.g. `lc 3604`). No parentheses around it.
- `<technique>` here is a **descriptive** phrase/clause (longer than the other targets — a short sentence is fine), e.g. `Dijkstra with time-windowed edges, arrival = max(t, start)+1`.
- Full example: `[add][review][shortest-path] solve lc 3112 - Dijkstra on undirected graph, skip node v when arrival >= disappear[v]`

`<technique>` = the **core idea**: max **5-6 words** (Targets A/B), **5-10 words** (Target C), or a **descriptive phrase/clause** (Target D).

## How to fill in NUMBER and technique
- **Read the source file(s)** in each unit first. Derive both the problem number and the technique from the **actual code** (the solved function + algorithm), NOT from the filename — filenames are often leftover templates and may not match the code inside.
- If the filename's number disagrees with the code, keep the filename as-is but use the **code's** real number in the message, and note the mismatch in your summary.

## Lint before staging (main repo, REQUIRED)

The main repo has pre-commit configured (`.pre-commit-config.yaml`: clang-format per
`.clang-format`, `g++ -std=c++20 -fsyntax-only`, whitespace/EOF hygiene). CI runs the same hooks
on every PR diff, so an unlinted commit will block auto-merge in `pr` mode.

- Before staging each Target A/D unit (after writing the annotation block):
  `pre-commit run --files <unit files>`. Fixer hooks modify files in place and report "failed" on
  the first pass — re-run until clean, then stage the fixed result. Lint fixes are part of the
  unit commit, **never** a separate commit.
- If the syntax check fails on a **missing `#include`** (the LeetCode template compiles there via
  transitive includes that libstdc++ on CI does not provide), add the missing include — that is a
  mechanical portability fix, not a solution change — and note it in the summary. Any **other**
  syntax failure means broken code → the empty/unsolved guardrail applies (flag, don't commit).
  - Never write `#include <bits/stdc++.h>` in a new solution — it is a GCC implementation detail
    and does not exist on Apple clang/libc++. Include the specific headers the code uses. The
    repo's existing files were converted to explicit includes, so a `bits/stdc++.h` failure now
    means a new file reintroduced it.
  - Repo-relative includes (e.g. `#include "topics/graph/disjoint_set_union.hpp"`) resolve because
    the hook passes `-I.`; a failure there is a wrong path, not a missing header.
- **Bulk-move exception:** when a unit is a pure file move / module rename of code that is already
  committed (git reports `R100`), do **not** run the fixer hooks over the moved files. Reformatting
  already-merged code destroys rename detection and buries the move in hundreds of unrelated lines.
  Commit the rename alone with `--no-verify`, use a `[chore][review][<topic>]` message that says the
  content is unchanged, and flag in the summary that those files remain unformatted (so CI lint will
  fail if that branch is later published — reformat them in their own dedicated commit first, after
  asking). This exception applies **only** to `R100` moves, never to new or edited solutions.
- Targets B/C (submodules) have no pre-commit config yet; commit them as-is.
- If `pre-commit` is missing on the machine, install it (`pipx install pre-commit`) instead of
  skipping lint.
- **Style hooks are scoped.** `.pre-commit-config.yaml` excludes archived areas
  (`daily/2024_*`, `daily/2025_*`, `contests/`, `cses/`, `vnoi/`, `codeforces/`) from
  clang-format and the whitespace fixers, because reformatting finished work buries the real
  change. Everything else — including `daily/2026_*` and all of `problems/` — is in scope, so a
  fixer hook may tidy a file you touch even though you only added a solution; that tidy-up
  belongs in the same unit commit. Never add a new path to that exclude list to dodge a lint
  failure. The syntax check is never excluded.

## Annotate each solution before committing (REQUIRED)

Before staging any solution file, add the annotation at the **top of the file** (right after the
`#include`s / imports, before the `Solution` class) as **three separate block comments**, in this
order. Use `/* … */` for C/C++ and the language's block-comment equivalent otherwise (`""" … """` for
Python).

**Block 1 — the problem** (or the `pull-lc` block, see the rules below):
- **Title line** — `LC <NUMBER> - <Title>` (or the Codeforces problem id/title for course units), plus
  the source URL.
- **Problem** — the problem statement, **fetched from the source** (see below), restated faithfully:
  what is given, what must be returned, and the **constraints** block.
- **Examples** — the source's worked examples (input / output / explanation), **fetched from the
  source**. Keep 2–3 representative ones — enough to pin down the exact I/O shape and at least one
  edge case; do not paste all of them if the source lists many.

**Block 2 — `Best solution`** — the reference-optimal algorithm, independent of the user's code:
- `Best solution: <name>` — the optimal (or near-optimal) algorithm for the problem.
- `Complexity:` — `Time:` and `Space:` in Big-O (e.g. `Time: O(V+E)`, `Space: O(V)`), with a
  one-clause justification each.
- `Approach (detail):` — a numbered, step-by-step explanation of *how* the best solution works and
  *why* it is correct (the key insight), detailed enough to reconstruct the solution from the comment.

**Block 3 — `Your solution`** — the algorithm the **committed code actually runs**, described on its
own terms, then judged against block 2:
- `Your solution: <name>` — a short name of the algorithm in the code. If the file holds several
  implementations, name the one the platform calls (the live method / the lambda it returns) and list
  the dead ones in one line (`Also in the file (not called): trivial_sol — …`).
- `Complexity:` — the **code's own** `Time:` and `Space:` in Big-O, with a one-clause justification
  each (not the best solution's numbers).
- `Approach:` — a numbered, step-by-step walk-through of what the committed code does, using its own
  variable / function names, so the reader can follow the code from the comment.
- `Verdict:` — exactly one of the three verdicts below, then the justification in the same sentence or
  the next one:
  - `MATCHES BEST` — the code implements the documented optimal algorithm.
  - `SUBOPTIMAL` — the code is correct but with worse complexity. Name the gap against block 2
    (e.g. `O(n^2) vs O(n log C)`) and say why it still passes (or does not) under the constraints.
  - `INCORRECT` — the code does not solve the problem. Name the failing case.
  - Mention how it was verified (brute-force / exhaustive test) when you tested it.

  Block 3 is about **the code in the file**, not the ideal — never write `MATCHES BEST` for a file
  whose live method is not the optimal one. Any verdict other than `MATCHES BEST` must also be
  repeated in the commit **summary**.

Layout (C++; indent body lines by 4 spaces like the pulled statement):
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
        2. ...
*/
/*
    Your solution: stack of '(' indices propagating doubled inner scores

    Complexity:
        Time:  O(n) — each character is pushed / popped once.
        Space: O(n) — the stack and score[].

    Approach:
        1. ...
        2. ...

    Verdict: SUBOPTIMAL — same O(n) time but O(n) vs O(1) extra space; irrelevant at n <= 50.
        Verified on all balanced strings up to length 18.
*/
```

### Fetching the problem statement + examples (REQUIRED)

The statement and examples must come from the **actual source**, not from memory. For LeetCode use
`python3 scripts/pull_leetcode_statement.py <N> --dry-run` (prints the verbatim block without editing;
`WebFetch` on leetcode.com pages returns 403). For other sources use `WebFetch`.
Resolve the source URL per target:
- **Target A (`src/leetcode/daily/`) and Target D (`src/leetcode/problems/`)** — LeetCode:
  `https://leetcode.com/problems/<slug>/`. Derive `<slug>` from the problem title in kebab-case
  (e.g. `3517. Smallest Palindromic Rearrangement I` → `smallest-palindromic-rearrangement-i`).
- **Target B (`courses/dsa-mentorship`)** — Codeforces. These files usually already carry the exact
  URL in a leading `//` comment (e.g. `// https://codeforces.com/group/.../problem/C`) — **use that
  URL**; it is authoritative for the group/contest-scoped statement.
- **Target C (`courses/fse13-faang`)** — LeetCode via the filename's 4-digit id (strip zero-padding
  to build the slug lookup). For homework-numbered files with no LC id, use whatever source URL the
  file references; if there is none, see the fallback below.

Fallback when the fetch fails (offline, 403/404, login-walled, or no resolvable URL): write the
statement and examples from your own knowledge of the problem, mark that section in the file with a
`(statement not verified against source — reconstructed)` note, and **flag the unit in your commit
summary**. Never silently present reconstructed text as sourced.

Rules for the annotation:
- Document the **objectively best** solution for the problem, even when the committed code uses a
  different / less-optimal approach (the user's code may not yet be optimal). When the committed code
  diverges from the documented best, that divergence goes in **both** places: the `Verdict:` of
  block 3 inside the file, and your commit **summary**.
- Identify the problem from the **actual code**, not the filename — then fetch *that* problem's
  statement. If the filename's id points at a different problem than the code solves, the statement
  must match the **code**.
- Keep the statement/examples **concise and verbatim in substance** — trim LeetCode's boilerplate
  ("Return the answer…" preamble, follow-up prompts, tag lists), but never reword the constraints or
  alter an example's numbers.
- The comment is part of the **same unit commit** (included in the `[add]`/`[update]`), never a
  separate commit. If a file was already committed this run without it, `--amend` that commit.
- If you cannot confidently determine the best solution, **ask** rather than guessing.
- **A statement already pulled by `pull-lc`** (a comment block starting `LC <N> - <Title>` for the
  code's real problem number) **is** block 1. Keep it **exactly as pulled** — do not trim it to 2–3
  examples or reword it — and write blocks 2 and 3 as their own `/* … */` blocks directly below it, so
  a later `pull-lc` re-run never wipes the analysis. If the pulled block is for the wrong problem
  (filename id ≠ code), re-pull with the code's real id via `--file`.
- When a commit touches a file that already carries the **older single-block analysis** (Best
  solution, Complexity, Approach and a one-paragraph `Your solution:` verdict in one comment), split
  it into the block 2 / block 3 layout above as part of that unit's commit.

## Guardrails
- **Do NOT add any `Co-Authored-By` / trailer line.** The user writes all the solution code; commits are authored solely by the user.
- **Never** `git add -A`; stage only the unit's own files.
- If a unit's solution is **empty/unsolved** or the code is broken, do NOT commit it silently — flag it and ask whether to skip, or wait for the user to finish it.
- If a unit is genuinely ambiguous (can't tell the problem/technique from the code), ask rather than guess.
- Committing inside a submodule leaves the **parent repo's submodule pointer** showing as modified — that's expected. Do **not** auto-commit the pointer bump in the parent; leave it for the user. This stays true in `push` / `pr` mode: the submodule *branch* gets pushed, the parent's pointer bump does not.
- **Pushing is authorized only by the mode word.** With no mode, nothing is pushed in any repo —
  do not offer to push as a bonus, just report the local commits.
- The only force-push you may ever run is `--force-with-lease`, and only on a branch you just
  rebased yourself. Never `--force`, and never push directly to `master` / `main`.
- `pr` mode may cause a merge into the base, but **only** via the `auto-merge` label + the CI bot.
  Never merge a PR yourself, in either repo kind.

## Publish modes — `push` / `pr`

`$ARGUMENTS` may contain exactly one publish mode. Publishing happens **after** all local unit
commits are done (local commits always happen, whatever the mode).

| mode | what happens |
|---|---|
| *(none)* | **Local commits only — the default.** Nothing is pushed anywhere. |
| `push` | **Get the branches onto the remote, nothing more.** Rebase + push **every branch that received a commit in this run**, in all three repos. No PR is opened. |
| `pr` | **Get the work merged.** Rebase + push, then open/refresh a PR, wait for the auto-merge bot, then rebase-sweep the remaining local topic branches onto the updated base and re-push them. Its branch set is **wider** than `push` — see below. |

- The mode is a **bare word** (`push` / `pr`), matched case-insensitively. Strip it from
  `$ARGUMENTS` first; the remaining tokens are scope filters exactly as before.
- The old `[lc]` / `[problem]` flags are **gone**. Those selected *targets*; the mode now applies to
  **every** branch this run touched, in **every** repo (submodules included). Unit→branch
  resolution by domain/topic is completely unchanged — see **Branch handling** above.
- If both words appear, treat it as `pr` and say so in the summary.

#### Which branches each mode acts on (`pr` is wider than `push`)

- **`push`** acts on the branches that **received a commit in this run**. A branch with nothing new
  to push is simply not its business.
- **`pr`** acts on **every eligible branch whose work is not yet in the base** — that is, each local
  `phuc-nguyen/*` branch (per repo) where `git -C <repo> rev-list --count <base>..<branch>` is
  **greater than 0** — *whether or not this run committed to it*. `pr` means "get the work merged",
  so a branch carrying unmerged commits from an **earlier** run is still in scope. Compute this set
  explicitly at the start of the publish flow; never assume it equals the set of branches this run
  touched.
- **`Everything up-to-date` is NOT a stop condition in `pr` mode.** A branch pushed by an earlier run
  has nothing left to push, but it may still have **no open PR** — which is exactly how work gets
  stranded on the remote for weeks. After every push attempt in `pr` mode, **always** go on to query
  the remote (`gh pr list --head <branch> --state open`) and open a PR if there is none. Treat the
  push result as irrelevant to whether step 2 runs.
  - A **merged** PR for that branch from an earlier run does not count as an open PR — new commits
    landed on the branch after that merge need a **new** PR.

### Per-repo facts the publish flow needs

**Never hardcode `origin`** — `courses/fse13-faang` pushes to a remote named **`fseorg`**. Resolve
the base branch and remote per repo before pushing:

| repo | base branch | remote |
|---|---|---|
| `.` (main repo) | `master` | `origin` |
| `courses/dsa-mentorship` | `main` | `origin` |
| `courses/fse13-faang` | `main` | `fseorg` |

Derive the remote from the base branch's upstream rather than trusting the table:
`git -C <repo> rev-parse --abbrev-ref <base>@{upstream}` → e.g. `fseorg/main`, so the remote is the
part before the `/`.

### Step 1 — rebase + push (both `push` and `pr`)

Resolve the mode's branch set first (see **Which branches each mode acts on** above): for `push`
the branches that received commits this run; for `pr` every local `phuc-nguyen/*` branch with
`rev-list --count <base>..<branch> > 0`. Group them by repo; for each repo, then each branch:

1. Update the base: `git -C <repo> fetch <remote>`, then
   `git -C <repo> checkout <base> && git -C <repo> merge --ff-only <remote>/<base>`.
   If the ff-only merge fails the local base has diverged → **stop for that repo**, publish nothing
   there, and report it.
2. `git -C <repo> checkout <branch> && git -C <repo> rebase <base>`. On any conflict:
   `git -C <repo> rebase --abort`, report that branch as **not published**, and continue with the
   remaining branches. Never resolve conflicts, never force through.
3. Push: `git -C <repo> push -u <remote> <branch>` when the branch has no upstream, else
   `git -C <repo> push --force-with-lease <remote> <branch>` (the rebase rewrote history). If
   `--force-with-lease` is rejected the remote branch moved underneath you → report it, do **not**
   retry with `--force`. `Everything up-to-date` is a **success**, not a skip — the branch stays in
   scope for step 2.

`push` mode stops here. Report the pushed branches and move on to **After committing**.

### Step 2 — open the PRs (`pr` mode only)

Run this for **every branch in the `pr` branch set**, including branches that step 3 reported as
`Everything up-to-date`. Only the **main repo** has the auto-merge bot (`.github/workflows/ci.yml`);
the submodules have no CI at all.

4. Main repo — ensure the opt-in label exists (idempotent):
   `gh label create auto-merge --color 0E8A16 --description "CI merges this PR when lint passes and there are no conflicts" 2>/dev/null || true`
5. Main repo, per branch: query `gh pr list --head <branch> --state open`. If it is empty →
   `gh pr create --base master --head <branch> --fill --label auto-merge`. **Query this for every
   branch in the set** — a branch that pushed clean, and a branch that was already up-to-date, are
   treated identically here. Otherwise the push in step 3 already updated the open PR — just ensure
   the label (`gh pr edit <number> --add-label auto-merge`).
6. Submodules, per branch: `gh pr create -R <owner>/<repo> --base main --head <branch> --fill`, with
   **no** `auto-merge` label — there is no CI there to honour it. Report the URL and state plainly
   that it waits for a **manual** merge by the user.

### Step 3 — wait for the auto-merge bot (`pr` mode, main repo only)

7. Poll each main-repo PR until it merges: `gh pr view <number> --json state,mergedAt,mergeStateStatus`,
   about every 20s for up to ~10 minutes total across all PRs.
   - `MERGED` → good, the base advanced.
   - Still `OPEN` at the deadline, or `CLOSED` without a merge, or the bot commented that the PR
     conflicts with `master` → record that branch as **published but not merged** and move on.
     **Never merge the PR yourself.**
8. Never wait on a submodule PR — nothing will merge it automatically.

### Step 4 — rebase sweep (`pr` mode only, and only after something actually merged)

A merge advances the base, so every other local topic branch now sits on stale history. That is what
this sweep fixes.

9. `git -C <repo> fetch <remote>` and fast-forward the base again as in step 1. **If the base did not
   move, skip the sweep entirely** and say so — there is nothing to rebase onto.
10. Eligible branches are **only** `phuc-nguyen/*` local branches. Never rewrite the base itself, and
    never touch a branch whose upstream *is* the base — the main repo has a `cheatsheet` branch
    tracking `origin/master`, and force-pushing that would rewrite master's remote ref.
11. For each eligible branch (including the ones just merged in this run):
    - `git rev-list --count <base>..<branch>` is `0` → the series is fully merged; bring it level with
      `git merge --ff-only <base>` and push it with a **plain** push (no force needed) if it has an
      upstream.
    - otherwise `git checkout <branch> && git rebase <base>`, then re-push as in step 3
      (`--force-with-lease`, since the rebase rewrote history). On conflict: `git rebase --abort`,
      leave that branch exactly as it was, report it, and continue with the next.
12. A branch with **no** upstream is rebased locally and **not** pushed — the sweep never publishes
    work that was never published before.
13. Run the sweep in the submodules too, but only when step 9 shows their base actually advanced
    (i.e. the user merged an earlier submodule PR themselves). Submodule PRs opened in this run never
    merge on their own, so normally the submodule sweep is a no-op.

## After committing
For each repo you committed in, print its `git log --oneline` for the new commits and the branch it's on, then a combined table of `repo | branch | unit | number | technique`, plus any flagged/skipped units and any branches you created (with their base).

Then, when a mode was passed:
- **`push` and `pr`** — a publish table `repo | branch | pushed? | how (new upstream / force-with-lease)`, and for anything not pushed, the reason (rebase conflict, diverged base, rejected lease).
- **`pr` only** — add `PR URL | merged?` per branch, marking submodule PRs as *manual merge, no CI*; then a sweep table `repo | branch | rebased or ff | re-pushed? | conflict?`, or an explicit line saying the sweep was skipped because the base never moved.
- **No mode** — say plainly that nothing was pushed and name the mode word (`push` / `pr`) that would publish it.

**Stranded-work check (every run, every mode).** Before finishing, list each local `phuc-nguyen/*`
branch per repo with `git -C <repo> rev-list --count <base>..<branch>` > 0 **and no open PR**. Report
them explicitly as *unmerged work with no PR*, with the commit count. `ahead > 0` is never "fine" —
it means committed study work that is not in the base yet. Never report such a branch as merely
"already rebased onto the latest base" and leave it at that.

Arguments: $ARGUMENTS
