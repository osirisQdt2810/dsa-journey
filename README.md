# 🧗 dsa-journey

> A personal, ever-growing archive of my **Data Structures & Algorithms** journey — daily LeetCode, weekly contests, classic problem sets, competitive-programming judges, structured courses, and a hand-written, reusable C++ algorithm library.

<p align="center">
  <img alt="C++17" src="https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white">
  <img alt="LeetCode" src="https://img.shields.io/badge/LeetCode-daily-FFA116?logo=leetcode&logoColor=white">
  <img alt="Codeforces" src="https://img.shields.io/badge/Codeforces-contests-1F8ACB?logo=codeforces&logoColor=white">
  <img alt="Solutions" src="https://img.shields.io/badge/solutions-236%2B-2ea44f">
  <img alt="Daily streak" src="https://img.shields.io/badge/daily%20folders-149-blueviolet">
</p>

---

## ✨ What is this?

This repo is my working notebook for getting good at algorithms — not a polished library, but a **living log** of everything I solve and learn. It has three layers:

1. **Solve** — solutions to problems from LeetCode, Codeforces, CSES, and VNOI (`src/`).
2. **Distill** — after solving enough of a kind, I extract the pattern into a clean, reusable, header-only C++ template (`topics/`).
3. **Study** — structured coursework tracked as git submodules (`courses/`).

Everything is committed with a consistent convention (see [Conventions](#-conventions)) so the git history itself reads like a study diary.

---

## 📊 At a glance

| Metric | Count |
|---|--:|
| Total C++ solutions | **236+** |
| Daily LeetCode folders | **149** (since Nov 2024) |
| LeetCode weekly contests | **8** |
| Codeforces contests | **3** |
| Classic problem topics | **13** |
| Reusable algorithm headers | **28** |

---

## 🗂️ Repository structure

```
dsa-journey/
├── src/                        # Raw solutions, grouped by judge
│   ├── leetcode/
│   │   ├── daily/              # One folder per day: YYYY_MM_DD/
│   │   ├── contests/           # Weekly/biweekly contest sets
│   │   └── problems/           # Curated sets grouped by technique
│   ├── codeforces/contests/    # CF rounds
│   ├── cses/                   # CSES problem set
│   └── vnoi/                   # Vietnamese Olympiad in Informatics
│
├── topics/                     # 📚 Reusable, header-only C++ algo library
│   ├── graph/                  # DSU, MST, shortest path, LCA, matching…
│   ├── tree/                   # Segment/Fenwick/BST/AVL/Trie…
│   ├── string/                 # Hashing, KMP/Z, matching…
│   ├── linear/                 # Stack, queue, sparse table, sqrt-decomp
│   └── arithmetic/             # Number theory / algebra
│
├── courses/                    # 🎓 Structured coursework (git submodules)
│   ├── dsa-mentorship/
│   └── fse13-faang/
│
├── scripts/                    # Tooling (see below)
│   └── find_unsolved_leetcode.py
│
└── .claude/commands/           # Custom Claude Code slash commands
    └── commit-progress.md      # /commit-progress workflow
```

---

## 🧩 The `topics/` library

The part I'm proudest of. Instead of copy-pasting boilerplate every contest, recurring algorithms live here as **generic, header-only** templates you can `#include` and reuse.

| Area | Headers |
|---|---|
| **Graph** | `disjoint_set_union` · `minimum_spanning_tree` · `shortest_path` · `lowest_common_ancestor` · `connectivity` · `topological_sort` · `bipartite_matching` · `traversal` |
| **Tree** | `segment_tree` · `fenwick_tree` · `binary_search_tree` · `avl_tree` · `trie` |
| **String** | `rolling_hash` · `pattern_search` · `string_matching` |
| **Linear** | `stack` · `queue` · `sparse_table` · `sqrt_decomp` |
| **Arithmetic** | `algebra` |

---

## 🏆 Practice by platform

| Platform | Where | Focus |
|---|---|---|
| **LeetCode** | `src/leetcode/` | Daily challenge, weekly contests, topic-grouped classics |
| **Codeforces** | `src/codeforces/contests/` | Competitive rounds |
| **CSES** | `src/cses/` | Problem-set fundamentals (DP, etc.) |
| **VNOI** | `src/vnoi/` | Vietnamese Olympiad in Informatics |

### Classic problem sets (`src/leetcode/problems/`)
Grouped by the technique they teach:

`data_structure_design` · `dsu` · `fenwick_tree` · `lca` · `lfu` · `lru` · `segtree` · `shortest_path` · `sqrt_decomposition` · `sweep_line` · `trie` · `two_pointers`

---

## 📅 Daily LeetCode

One folder per day under `src/leetcode/daily/YYYY_MM_DD/`, each holding that day's solution. Commits follow a strict, greppable format so the log doubles as a technique index:

```
[daily] add leetcode daily challenge DD-MM-YYYY (lc <NUMBER> - <technique>)
```

Example:
```
[daily] add leetcode daily challenge 11-07-2026 (lc 2685 - DSU component edge count)
```

---

## 🎓 Courses (git submodules)

Coursework is tracked as independent repositories via git submodules, so clone with `--recursive`:

```bash
git clone --recursive <this-repo-url>
# already cloned?
git submodule update --init --recursive
```

| Submodule | Path |
|---|---|
| DSA Mentorship | `courses/dsa-mentorship` |
| FSE13 · FAANG prep | `courses/fse13-faang` |

---

## 🛠️ Tooling

### `scripts/find_unsolved_leetcode.py`
Queries LeetCode (via its GraphQL API) to surface **unsolved** problems filtered by topic/keyword — so I can target weak areas instead of picking randomly.

### `/commit-progress` (Claude Code command)
A custom slash command (`.claude/commands/commit-progress.md`) that commits study progress across all three repos — the main repo's daily LeetCode **and** the two course submodules — one commit per unit, on the correct per-repo branch, using the exact message conventions above. Never stages out-of-scope files, never force-adds, never pushes on its own.

---

## ⚙️ Building & running

Solutions are self-contained single translation units — compile whichever file you want directly:

```bash
g++ -std=c++17 -O2 -Wall "src/leetcode/daily/2026_07_11/2685. Count the Number of Complete Components.cpp" -o /tmp/sol && /tmp/sol
```

To pull in a reusable header from `topics/`, add its folder to the include path:

```bash
g++ -std=c++17 -O2 -Itopics your_solution.cpp -o sol
```

> `build/` holds local compile artifacts and is git-ignored.

---

## 🧭 Conventions

- **One unit per commit** — a single day / problem / lesson, never a bulk dump.
- **Branch per stream** — daily, contests, reviews, and each course lesson each live on their own `phuc-nguyen/*` feature branch; `master` only ever receives merges.
- **Message = technique index** — every commit names the LC number and the *core idea* (≤ 5–6 words), so `git log` is searchable by algorithm.
- **Solutions are mine** — no co-author trailers; commits are authored solely by me.

---

<p align="center"><i>Solve → distill → reuse. One problem a day. 🚀</i></p>
