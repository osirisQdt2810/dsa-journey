---
description: Commit daily LeetCode challenge folders under src/leetcode/daily (one commit per day)
argument-hint: "[optional: specific date folders e.g. 2026_07_06, else all uncommitted]"
allowed-tools: Bash(git status:*), Bash(git add:*), Bash(git commit:*), Bash(git log:*), Read
---

Commit the daily LeetCode challenge solutions under `src/leetcode/daily/`, following the exact convention below.

## Scope
- Only touch folders under `src/leetcode/daily/`. If `$ARGUMENTS` names specific date folders (e.g. `2026_07_06`), commit only those; otherwise commit every uncommitted daily folder.
- Find them with `git status --porcelain src/leetcode/daily/`.
- **Never** run `git add -A` or stage anything outside the target daily folders. Leave other changes (e.g. `courses/*` submodules, `src/leetcode/problems/**`) untouched.

## One commit per day
Each date folder (`YYYY_MM_DD`) is its own commit. Commit message template:

```
[daily] add leetcode daily challenge DD-MM-YYYY (lc <NUMBER> - <main technique>)
```

- Date: convert the folder `YYYY_MM_DD` → `DD-MM-YYYY`.
- `<main technique>`: the core idea used to solve it, **max 5-6 words**, concise.

## How to fill in NUMBER and technique
- **Read the `.cpp` file(s)** in each folder first. Derive both the LC number and the technique from the **actual code content** (the solved function + algorithm), NOT from the filename — filenames are often leftover templates and may not match the code inside.
- If the filename's LC number disagrees with the code, keep the filename as-is but use the **code's** real LC number in the commit message, and note the mismatch in your summary.

## Guardrails
- **Do NOT add any `Co-Authored-By` / trailer line.** The user writes all the solution code; commits are authored solely by the user.
- If a folder's solution is **empty/unsolved** or the code is broken, do NOT commit it silently — flag it and ask whether to skip, or wait for the user to finish it.
- If a folder is genuinely ambiguous (can't tell the LC problem/technique from the code), ask rather than guess.
- Do **not** push. Only push (or force-push) if the user explicitly asks.

## After committing
Print `git log --oneline` for the new commits and a short table of `date | lc | technique`, plus any flagged/skipped folders.

Arguments: $ARGUMENTS
