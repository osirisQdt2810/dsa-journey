#!/usr/bin/env python3
"""
Find unsolved LeetCode problems by topic (medium/hard only).

Setup (one time):
    pip install playwright requests && playwright install chromium

Usage:
    python find_unsolved_leetcode.py --topics shortest-path mst
    python find_unsolved_leetcode.py --topics two-pointers sliding-window --count 10

Session (pick ONE):
    A. Browser login (auto):  python find_unsolved_leetcode.py --topics ...
                              (Chromium opens, log in, session saved to scripts/.session)
    B. Manual cookie:         Open leetcode.com → DevTools → Network → any /graphql request
                              → Request Headers → copy LEETCODE_SESSION value
                              → echo '<value>' > scripts/.session
    C. Re-login:              python find_unsolved_leetcode.py --relogin --topics ...
"""

import argparse
import sys
import pathlib
import requests

GRAPHQL_URL = "https://leetcode.com/graphql"
SESSION_FILE = pathlib.Path(__file__).parent / ".session"

TOPIC_ALIASES = {
    # --- Two pointers / sliding window ---
    "two-pointers":               "two-pointers",
    "two-pointer":                "two-pointers",
    "sliding-window":             "sliding-window",

    # --- Graph traversal ---
    "graph":                      "graph",
    "bfs":                        "breadth-first-search",
    "breadth-first-search":       "breadth-first-search",
    "dfs":                        "depth-first-search",
    "depth-first-search":         "depth-first-search",

    # --- Graph algorithms ---
    "shortest-path":              "shortest-path",
    "mst":                        "minimum-spanning-tree",
    "minimum-spanning-tree":      "minimum-spanning-tree",
    "topo":                       "topological-sort",
    "topological-sort":           "topological-sort",
    "lca":                        "keyword:lowest common ancestor",
    "lowest-common-ancestor":     "keyword:lowest common ancestor",
    "scc":                        "strongly-connected-component",
    "strongly-connected":         "strongly-connected-component",
    "euler":                      "eulerian-circuit",
    "eulerian-circuit":           "eulerian-circuit",
    "biconnected":                "biconnected-component",

    # --- Union-Find ---
    "dsu":                        "union-find",
    "union-find":                 "union-find",

    # --- Trees ---
    "binary-tree":                "binary-tree",
    "binary-search-tree":         "binary-search-tree",
    "bst":                        "binary-search-tree",
    "balanced-bst":               "binary-search-tree",

    # --- Monotonic structures ---
    "monotonic-stack":            "monotonic-stack",
    "monotonic-queue":            "monotonic-queue",

    # --- Advanced data structures ---
    "trie":                       "trie",
    "segment-tree":               "segment-tree",
    "fenwick-tree":               "binary-indexed-tree",
    "binary-indexed-tree":        "binary-indexed-tree",
    "bit":                        "binary-indexed-tree",
    "heap":                       "heap-priority-queue",
    "priority-queue":             "heap-priority-queue",
    "ordered-set":                "ordered-set",

    # --- Basic data structures ---
    "stack":                      "stack",
    "queue":                      "queue",
    "linked-list":                "linked-list",
    "hash-table":                 "hash-table",
    "hash":                       "hash-table",

    # --- DP ---
    "dp":                         "dynamic-programming",
    "dynamic-programming":        "dynamic-programming",
    "memoization":                "memoization",
    "bitmask":                    "bitmask",
    "bitmask-dp":                 "bitmask",

    # --- Search / divide ---
    "binary-search":              "binary-search",
    "divide-and-conquer":         "divide-and-conquer",
    "backtracking":               "backtracking",
    "recursion":                  "recursion",
    "greedy":                     "greedy",

    # --- Bit manipulation ---
    "bit-manipulation":           "bit-manipulation",

    # --- String ---
    "string":                     "string",
    "string-matching":            "string-matching",
    "rolling-hash":               "rolling-hash",
    "suffix-array":               "suffix-array",
    "z-function":                 "string-matching",
    "kmp":                        "string-matching",

    # --- Math ---
    "math":                       "math",
    "number-theory":              "number-theory",
    "combinatorics":              "combinatorics",
    "geometry":                   "geometry",
    "line-sweep":                 "line-sweep",
    "game-theory":                "game-theory",
    "probability":                "probability-and-statistics",

    # --- Sorting ---
    "sorting":                    "sorting",
    "counting-sort":              "counting-sort",
    "radix-sort":                 "radix-sort",
    "merge-sort":                 "merge-sort",
    "bucket-sort":                "bucket-sort",

    # --- Misc ---
    "simulation":                 "simulation",
    "interactive":                "interactive",
    "data-stream":                "data-stream",
    "randomized":                 "randomized",
    "reservoir-sampling":         "reservoir-sampling",
    "matrix":                     "matrix",
    "prefix-sum":                 "prefix-sum",
    "difference-array":           "prefix-sum",
}

DIFFICULTIES = ["MEDIUM", "HARD"]

PROBLEM_LIST_QUERY = """
query problemsetQuestionList($categorySlug: String, $limit: Int, $skip: Int, $filters: QuestionListFilterInput) {
    problemsetQuestionList: questionList(
        categorySlug: $categorySlug
        limit: $limit
        skip: $skip
        filters: $filters
    ) {
        total: totalNum
        questions: data {
            frontendQuestionId: questionFrontendId
            title
            titleSlug
            difficulty
            acRate
            isPaidOnly
            status
            topicTags { slug }
        }
    }
}
"""


# ---------------------------------------------------------------------------
# Auth via Playwright
# ---------------------------------------------------------------------------

def login_via_browser() -> str:
    """Open a real Chromium window, wait for user to log in, return session cookie."""
    try:
        from playwright.sync_api import sync_playwright, TimeoutError as PWTimeout
    except ImportError:
        print("Playwright is not installed. Run:\n  pip install playwright && playwright install chromium")
        sys.exit(1)

    print("Opening browser — log in to LeetCode, then the window will close automatically.")
    with sync_playwright() as pw:
        browser = pw.chromium.launch(headless=False, slow_mo=50)
        ctx = browser.new_context()
        page = ctx.new_page()
        page.goto("https://leetcode.com/accounts/login/")

        # Wait until user is redirected away from /accounts/login/
        print("Waiting for login... (you have 3 minutes)")
        try:
            page.wait_for_url(
                lambda url: "leetcode.com/accounts/login" not in url,
                timeout=180_000,
            )
        except PWTimeout:
            print("Timed out waiting for login.")
            browser.close()
            sys.exit(1)

        page.wait_for_load_state("networkidle")
        cookies = {c["name"]: c["value"] for c in ctx.cookies()}
        browser.close()

    session = cookies.get("LEETCODE_SESSION")
    if not session:
        print("Login failed: LEETCODE_SESSION cookie not found.")
        sys.exit(1)

    SESSION_FILE.write_text(session)
    print(f"Session saved to {SESSION_FILE} — next runs won't need login.")
    return session


def get_session(force_relogin: bool = False) -> str:
    if not force_relogin and SESSION_FILE.exists():
        session = SESSION_FILE.read_text().strip()
        if session:
            return session
    return login_via_browser()


# ---------------------------------------------------------------------------
# LeetCode GraphQL
# ---------------------------------------------------------------------------

def make_headers(session: str) -> dict:
    return {
        "Content-Type": "application/json",
        "Cookie": f"LEETCODE_SESSION={session}",
        "Referer": "https://leetcode.com",
        "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36",
        "x-csrftoken": "dummy",
    }


def fetch_problems(tag_slug: str, difficulty: str, session: str) -> list:
    variables = {
        "categorySlug": "",
        "skip": 0,
        "limit": 100,
        "filters": {"tags": [tag_slug], "difficulty": difficulty},
    }
    resp = requests.post(
        GRAPHQL_URL,
        json={"query": PROBLEM_LIST_QUERY, "variables": variables},
        headers=make_headers(session),
        timeout=15,
    )
    resp.raise_for_status()
    return resp.json().get("data", {}).get("problemsetQuestionList", {}).get("questions", [])


_KEYWORD_CACHE: dict = {}

def fetch_all_by_keyword(keyword: str, session: str) -> list:
    """Paginate through ALL results for a keyword, preserving API (relevance) order."""
    if keyword in _KEYWORD_CACHE:
        return _KEYWORD_CACHE[keyword]
    PAGE = 50
    all_results = []
    skip = 0
    while True:
        variables = {
            "categorySlug": "",
            "skip": skip,
            "limit": PAGE,
            "filters": {"searchKeywords": keyword},
        }
        resp = requests.post(
            GRAPHQL_URL,
            json={"query": PROBLEM_LIST_QUERY, "variables": variables},
            headers=make_headers(session),
            timeout=15,
        )
        resp.raise_for_status()
        data = resp.json().get("data", {}).get("problemsetQuestionList", {})
        page = data.get("questions", [])
        total = data.get("total", 0)
        all_results.extend(page)
        skip += PAGE
        if skip >= total or not page:
            break
    _KEYWORD_CACHE[keyword] = all_results
    return all_results


def fetch_problems_by_keyword(keyword: str, difficulty: str, session: str) -> list:
    all_problems = fetch_all_by_keyword(keyword, session)
    return [p for p in all_problems if p.get("difficulty", "").upper() == difficulty]


def session_is_valid(session: str) -> bool:
    try:
        problems = fetch_problems("shortest-path", "MEDIUM", session)
        # If status field is None for all (unauthenticated), session is expired
        return any(p.get("status") is not None for p in problems[:5]) or True
    except Exception:
        return False


def _distribute_quota(tags: list, total: int) -> dict:
    """Randomly distribute `total` across tags with ≤random variation (10-20%) between quotas."""
    import random
    n = len(tags)
    if n == 1:
        return {tags[0]: total}
    max_var = random.uniform(0.10, 0.20)
    base = total / n
    quotas = {}
    remaining = total
    for tag in tags[:-1]:
        delta = random.uniform(-max_var, max_var) * base
        q = max(1, round(base + delta))
        quotas[tag] = q
        remaining -= q
    quotas[tags[-1]] = max(1, remaining)
    return quotas


def collect_unsolved(tag_slugs: list, session: str, target: int = 20, medium_ratio: float = 0.67) -> list:
    quotas = _distribute_quota(tag_slugs, target)
    print(f"  Quota per tag: { {t: quotas[t] for t in tag_slugs} }")

    seen_ids = set()
    all_results = []

    for tag in tag_slugs:
        quota = quotas[tag]
        n_med = round(quota * medium_ratio)
        n_hard = quota - n_med

        is_keyword = tag.startswith("keyword:")
        pools = {"MEDIUM": [], "HARD": []}
        for diff in ("MEDIUM", "HARD"):
            print(f"  [{diff}] {tag} ...", flush=True)
            try:
                if is_keyword:
                    problems = fetch_problems_by_keyword(tag[len("keyword:"):], diff, session)
                else:
                    problems = fetch_problems(tag, diff, session)
            except Exception as e:
                print(f"  ERROR: {e}", file=sys.stderr)
                continue
            for p in problems:
                qid = p["frontendQuestionId"]
                if qid in seen_ids or p.get("isPaidOnly"):
                    continue
                if p.get("status") != "ac":
                    pools[diff].append(p)

        if not is_keyword:
            # Tag search: sort by ID (stable ordering)
            for diff in pools:
                pools[diff].sort(key=lambda q: int(q["frontendQuestionId"]))
        # Keyword search: keep API relevance order (most relevant first)

        n_take = min(quota, len(pools["MEDIUM"]) + len(pools["HARD"]))
        n_med = min(round(quota * medium_ratio), len(pools["MEDIUM"]))
        n_hard = min(quota - n_med, len(pools["HARD"]))
        # fill leftover from the other bucket
        if n_med + n_hard < n_take:
            n_med = min(n_take - n_hard, len(pools["MEDIUM"]))

        selected = pools["MEDIUM"][:n_med] + pools["HARD"][:n_hard]
        for p in selected:
            seen_ids.add(p["frontendQuestionId"])
            all_results.append(p)

        # Track shortfall — will be filled from leftovers below
        shortfall = quota - len(selected)
        quotas["__deficit__"] = quotas.get("__deficit__", 0) + shortfall

        # Accumulate leftovers (unseen problems beyond quota)
        used = {p["frontendQuestionId"] for p in selected}
        for diff in ("MEDIUM", "HARD"):
            for p in pools[diff]:
                if p["frontendQuestionId"] not in used:
                    seen_ids.discard(p["frontendQuestionId"])  # allow reuse in leftover pool
                    quotas.setdefault("__leftovers__", []).append(p)

    # Fill deficit from accumulated leftovers
    deficit = quotas.get("__deficit__", 0)
    if deficit > 0:
        leftovers = [p for p in quotas.get("__leftovers__", [])
                     if p["frontendQuestionId"] not in seen_ids]
        # deduplicate preserving order
        seen_lt = set()
        unique_lt = []
        for p in leftovers:
            if p["frontendQuestionId"] not in seen_lt:
                seen_lt.add(p["frontendQuestionId"])
                unique_lt.append(p)
        med_lt = [p for p in unique_lt if p["difficulty"] == "Medium"]
        hard_lt = [p for p in unique_lt if p["difficulty"] == "Hard"]
        n_med = min(round(deficit * medium_ratio), len(med_lt))
        n_hard = min(deficit - n_med, len(hard_lt))
        for p in med_lt[:n_med] + hard_lt[:n_hard]:
            all_results.append(p)

    all_results.sort(key=lambda q: int(q["frontendQuestionId"]))
    return all_results[:target]


def print_table(problems: list):
    if not problems:
        print("No unsolved problems found.")
        return

    header = f"{'#':<5} {'ID':<6} {'Diff':<8} {'AC%':>6}  {'Title':<50}  URL"
    print("\n" + header)
    print("-" * 120)
    for i, p in enumerate(problems, 1):
        ac = f"{p['acRate']:.1f}%" if p.get("acRate") else "  n/a"
        url = f"https://leetcode.com/problems/{p['titleSlug']}/"
        print(f"{i:<5} {p['frontendQuestionId']:<6} {p['difficulty']:<8} {ac:>6}  {p['title']:<50}  {url}")


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--topics", nargs="+", default=["shortest-path", "mst"],
                        help="Topics: " + ", ".join(TOPIC_ALIASES))
    parser.add_argument("--count", type=int, default=20, help="Number of problems (default 20)")
    parser.add_argument("--relogin", action="store_true", help="Force re-login even if session is cached")
    args = parser.parse_args()

    session = get_session(force_relogin=args.relogin)
    tag_slugs = [TOPIC_ALIASES.get(t.lower(), t) for t in args.topics]

    print(f"\nSearching {args.count} unsolved problems: {tag_slugs}")
    problems = collect_unsolved(tag_slugs, session, target=args.count)
    print_table(problems)


if __name__ == "__main__":
    main()
