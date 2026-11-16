---
description: LeetCode helpers for a solution file (no commit) — `pull` writes the problem statement verbatim as a comment block, `test` turns the official examples into a runnable PASS/FAIL main()
argument-hint: "pull|test [pull|test] <ID> [<file>] [<ID> [<file>]]... — e.g. `pull 1606`, `test 3105`, `pull test 3105`"
model: sonnet
allowed-tools: Bash(python3 scripts/leetcode.py:*), Bash(g++:*), Bash(python3 -m py_compile:*), Bash(ls:*), Read, Edit
---

Parse `$ARGUMENTS`: the leading `pull` and/or `test` words are the subcommands (both given → run
`pull` first, then `test`, for every id); the remaining tokens are LeetCode ids, each optionally
followed by a file path that pins the target file. No subcommand word → ask which one.

Neither subcommand discovers other work, switches branches, lints, commits or pushes. The edited file
stays uncommitted and is committed later with its solution by a normal `/commit-progress` run.

**Target file** (both subcommands): the single file under `src/leetcode/` or `courses/` named
`<ID>. *` (zero-padding allowed). If several match (e.g. a daily copy and a `problems/` copy) or none
does, prefer the file the user has open in the IDE when its content is that problem; otherwise ask,
then pass it with `--file`.

## `pull` — statement into the file

For each id run `python3 scripts/leetcode.py pull <ID> [--file <path>]`. The script resolves the id →
slug, fetches the statement HTML from LeetCode's GraphQL `question` query, and writes it as a comment
block **verbatim** — every sentence, every example, the constraints and any follow-up, with
`<sup>`/`<sub>` rendered as `^`/`_`. **Images are dropped; nothing else is.** Never edit, trim or
reword the pulled text by hand.

- **Placement:** an earlier pulled block for the same id is replaced in place (idempotent, keeps any
  `@status/@time/@wa/@created` lines); otherwise the first empty `/* */` placeholder is filled;
  otherwise a new block goes right after `using namespace std;`. Its first lines are
  `LC <ID> - <Title>` and the problem URL.
- The script warns when LeetCode's C++ method name does not appear in the file — the filename's id
  names a different problem than the code solves. Relay the warning; do not "fix" it.
- Fetch fails (offline, premium-only) → report it and stop; never write a statement from memory.
- Report per id: the file touched and how (filled placeholder / replaced / inserted).

## `test` — example-based harness

Goal: turn the problem's **official examples** into a runnable entry point, so the user only has to
compile/run it and read PASS/FAIL. LeetCode prints examples in a Python-like literal syntax
(`[[1,2],[3,4]]`, `"abc"`, `true`, `null`); translate them into the **file's own language**.

1. **Examples, verbatim** — from the file's pulled `LC <ID> - …` block if present, else from
   `python3 scripts/leetcode.py pull <ID> --dry-run` (no file edit). Use **every** example with inputs
   and expected outputs exactly as printed. Neither available (fetch failed, premium) → stop and
   report; never invent examples or expected outputs.
2. **Signature from the code** — read the file's `Solution` (or design class) for the method name,
   parameter types (including `&` / mutability) and return type. Call the user's real method; never
   rewrite or "fix" the solution.
3. **Write the harness** between the markers `test-lc:begin` / `test-lc:end` (line comments:
   `// test-lc:begin` in C++/Java/Go/JS, `# test-lc:begin` in Python). A re-run replaces the marked
   region in place; helpers outside it are not touched. A region holding a harness for a **different**
   problem (a file copied from an earlier day) is replaced too.
   - **C++**: the region is the body of `main()` (keep `Solution sol;`-style setup inside it; leave the
     `auto init = …` lambda alone). Declare each input as a named local
     (`vector<vector<int>> positions = {{1,2},{2,3}};`) so `T&` parameters bind. Add any standard
     header the harness needs as a specific `#include` (never `<bits/stdc++.h>`).
   - **Python**: an `if __name__ == "__main__":` block. **Java**: `public static void main`. Other
     languages: their idiomatic entry point.
   - One fresh `Solution` per example (no state leaking between examples).
   - One line per example: `Example <i>: PASS` or `Example <i>: FAIL  got=<…>  expected=<…>`, with a
     small generic printer for vectors / nested vectors / strings, and a final
     `<passed>/<total> passed` line.
   - **Comparisons:** exact by default; floating point within `1e-5`. "In any order" → sort both sides
     before comparing. **Any** valid answer accepted (several correct outputs) → print `got` with
     `CHECK MANUALLY` instead of a verdict.
   - **Special input types:** `ListNode` / `TreeNode` → small `build_list` / `build_tree` (level-order
     with `null`) and matching serializers, only when the signature needs them. **Design problems**
     (`["MyCalendar","book",…]`, `[[],[10,20],…]`) → replay the operations on one object and compare
     each returned value, treating `null` as "no return value".
4. **Check it compiles** — C++: `g++ -std=c++20 -fsyntax-only <file>`; Python:
   `python3 -m py_compile`. An error in the harness → fix the harness. An error in the user's solution
   (unfinished code) → leave the solution untouched and report it. Do **not** run the tests — the user
   runs them.
5. **Report** per id: the file, how many examples were wired up, any `CHECK MANUALLY` examples, and
   the one-line command to run it, e.g. `g++ -std=c++20 -O2 "<file>" -o /tmp/a && /tmp/a`.
