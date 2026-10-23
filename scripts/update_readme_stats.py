#!/usr/bin/env python3
"""Refresh the auto-derived numbers and lists in README.md from what git actually tracks.

Usage:
    python3 scripts/update_readme_stats.py            # rewrite README.md in place
    python3 scripts/update_readme_stats.py --check    # exit 1 if README.md is out of date
    python3 scripts/update_readme_stats.py --ref origin/master

Counts come from `git ls-tree` of --ref (default HEAD), so untracked placeholders and unmerged
work never inflate them. What is rewritten:
    - the "solutions" and "daily folders" badges,
    - the "At a glance" table rows,
    - the backtick list of classic problem sets (src/leetcode/problems/<topic>/).
Prose sections (topics/ header table, Tooling) are not rewritten; the script only WARNS when a
header in topics/ or a script in scripts/ is not mentioned anywhere in the README.
"""

import argparse
import re
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
README = REPO / "README.md"
MONTHS = "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec".split()
# headers that only hold shared types / umbrella includes, not an algorithm of their own
AUX_HEADERS = {"graph", "graph_types", "string", "string_types", "tree", "tree_types", "utility"}


def tracked(ref):
    out = subprocess.run(["git", "-C", str(REPO), "ls-tree", "-r", "--name-only", ref],
                         capture_output=True, text=True, check=True).stdout
    return out.splitlines()


def subdirs(paths, prefix):
    """Distinct first-level directory names under `prefix/` that contain tracked files."""
    names = set()
    for p in paths:
        if p.startswith(prefix + "/"):
            rest = p[len(prefix) + 1:]
            if "/" in rest:
                names.add(rest.split("/", 1)[0])
    return sorted(names)


def collect(ref):
    paths = tracked(ref)
    daily = subdirs(paths, "src/leetcode/daily")
    first = daily[0] if daily else None
    since = f"{MONTHS[int(first[5:7]) - 1]} {first[:4]}" if first else "?"
    return {
        "solutions": sum(p.startswith("src/") and p.endswith(".cpp") for p in paths),
        "daily": len(daily),
        "since": since,
        "lc_contests": len(subdirs(paths, "src/leetcode/contests")),
        "cf_contests": len(subdirs(paths, "src/codeforces/contests")),
        "problem_topics": subdirs(paths, "src/leetcode/problems"),
        "headers": sorted(Path(p).stem for p in paths
                          if p.startswith("topics/") and p.endswith(".hpp")),
        "scripts": sorted(Path(p).name for p in paths
                          if p.startswith("scripts/") and p.endswith(".py")),
    }


def sub_once(pattern, repl, text, what):
    new, n = re.subn(pattern, repl, text, count=1, flags=re.M)
    if n != 1:
        sys.exit(f"error: could not find the {what} in README.md (pattern {pattern!r})")
    return new


def render(text, s):
    algo_headers = [h for h in s["headers"] if h not in AUX_HEADERS]
    text = sub_once(r"(badge/solutions-)\d+(%2B)", rf"\g<1>{s['solutions']}\g<2>", text,
                    "solutions badge")
    text = sub_once(r"(badge/daily%20folders-)\d+", rf"\g<1>{s['daily']}", text,
                    "daily folders badge")
    rows = {
        "Total C++ solutions": f"**{s['solutions']}+**",
        "Daily LeetCode folders": f"**{s['daily']}** (since {s['since']})",
        "LeetCode weekly contests": f"**{s['lc_contests']}**",
        "Codeforces contests": f"**{s['cf_contests']}**",
        "Classic problem topics": f"**{len(s['problem_topics'])}**",
        "Reusable algorithm headers": f"**{len(s['headers'])}**",
    }
    for label, value in rows.items():
        text = sub_once(rf"^(\| {re.escape(label)} \| ).*?( \|)$", rf"\g<1>{value}\g<2>", text,
                        f"'{label}' row")
    topics_line = " · ".join(f"`{t}`" for t in s["problem_topics"])
    text = sub_once(r"(Grouped by the technique they teach:\n\n)`.*`$", rf"\g<1>{topics_line}",
                    text, "classic problem set list")
    return text, algo_headers


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--ref", default="HEAD", help="git ref to count from (default HEAD)")
    ap.add_argument("--check", action="store_true", help="exit 1 if README.md is outdated")
    args = ap.parse_args()

    stats = collect(args.ref)
    old = README.read_text()
    new, algo_headers = render(old, stats)

    for h in algo_headers:
        if f"`{h}`" not in new:
            print(f"warning: header topics/**/{h}.hpp is not listed in the README topics table")
    for sc in stats["scripts"]:
        if sc not in new:
            print(f"warning: scripts/{sc} is not mentioned in the README Tooling section")

    if new == old:
        print("README.md is up to date")
        return
    if args.check:
        print("README.md is out of date — run scripts/update_readme_stats.py")
        sys.exit(1)
    README.write_text(new)
    print(f"README.md updated: {stats['solutions']} solutions, {stats['daily']} daily folders, "
          f"{len(stats['problem_topics'])} problem topics, {len(stats['headers'])} headers")


if __name__ == "__main__":
    main()
