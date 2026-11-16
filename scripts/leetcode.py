#!/usr/bin/env python3
"""All LeetCode tooling for this repo, as subcommands of one script (stdlib only).

Usage:
    python3 scripts/leetcode.py pull <ID> [--file PATH] [--dry-run]
    python3 scripts/leetcode.py attempt <FILE>... [--json]
    python3 scripts/leetcode.py attempt <FILE> --write [--status S] [--time MIN] [--wa N]
    python3 scripts/leetcode.py daily [DATE ...] [--from DATE --to DATE]
    python3 scripts/leetcode.py unsolved --topics two-pointers sliding-window [--count 20]
    python3 scripts/leetcode.py feed [--repo PATH] [--out PATH] [--offline] [--dry-run]
    python3 scripts/leetcode.py login

pull      Pull a problem's statement verbatim into its solution file as a comment block.
attempt   Work out a solution's attempt metadata: @created (file birth time), @time (minutes from
          @created to the first Accepted submission after it) and @wa (rejected submissions in
          between). --write records @status/@time/@wa/@created in the file.
daily     Scaffold src/leetcode/daily/YYYY_MM_DD/ for LeetCode's daily challenge of each date:
          the repo template, LeetCode's C++ signature and the pulled statement.
unsolved  List problems you have not solved yet, filtered by topic (medium/hard only).
feed      Export every LeetCode attempt in the archive as the JSON feed of the Omnia
          "cultivation" Anki plugin (run as the last step of /commit-progress).
login     Refresh scripts/.session (LEETCODE_SESSION) through a real browser (needs playwright).

Subcommands that read your account (attempt, unsolved) need scripts/.session (git-ignored).
Step-by-step instructions for copying the LEETCODE_SESSION cookie live at the top of that file;
`login` fetches it through a browser instead, `login --check` tells whether it is still valid.
"""

import argparse
import html
import json
import math
import os
import re
import subprocess
import sys
import tempfile
import time
import urllib.error
import urllib.request
from datetime import date, datetime, timedelta
from html.parser import HTMLParser
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
SESSION_FILE = Path(__file__).resolve().parent / ".session"
SEARCH_ROOTS = [REPO / "src" / "leetcode", REPO / "courses"]
DAILY_ROOT = REPO / "src" / "leetcode" / "daily"
GRAPHQL_URL = "https://leetcode.com/graphql"
LC_ALL_URL = "https://leetcode.com/api/problems/all/"
SOLUTION_EXTS = (".cpp", ".cc", ".h", ".hpp", ".py")
INDENT = "    "
# LeetCode answers 403 to a bare "Mozilla/5.0" UA; send a full browser UA + Referer.
UA = {
    "User-Agent": (
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 "
        "(KHTML, like Gecko) Chrome/126.0 Safari/537.36"
    ),
    "Referer": "https://leetcode.com/",
    "Content-Type": "application/json",
}


def warn(msg):
    print(f"warning: {msg}", file=sys.stderr)


def rel(path):
    path = Path(path).resolve()
    return path.relative_to(REPO) if path.is_relative_to(REPO) else path


# =============================================================================================
# HTTP / GraphQL / session
# =============================================================================================


def http_json(url, payload=None, session=None, timeout=30):
    headers = dict(UA)
    if session:
        # Matching csrftoken cookie + header satisfies LeetCode's CSRF check for queries.
        headers["Cookie"] = f"LEETCODE_SESSION={session}; csrftoken=dummy"
        headers["x-csrftoken"] = "dummy"
    data = json.dumps(payload).encode() if payload is not None else None
    req = urllib.request.Request(url, data=data, headers=headers)
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return json.load(resp)


def graphql(query, variables=None, session=None):
    out = http_json(GRAPHQL_URL, {"query": query, "variables": variables or {}}, session)
    if out.get("errors") and not out.get("data"):
        raise RuntimeError(out["errors"][0].get("message", "GraphQL error"))
    return out["data"]


SESSION_HELP = """\
# LeetCode session cookie for scripts/leetcode.py (`attempt`, `unsolved`).
# Lines starting with '#' are ignored; the first other non-empty line is the cookie value.
# This file is git-ignored: never commit or share it (it logs in as you).
#
# How to get LEETCODE_SESSION:
#   1. Log in at https://leetcode.com in your browser.
#   2. Open DevTools: Chrome/Edge Cmd+Opt+I (Ctrl+Shift+I), Firefox Cmd+Opt+I,
#      Safari: Settings -> Advanced -> "Show features for web developers", then Cmd+Opt+I.
#   3. Chrome/Edge: Application tab -> Storage -> Cookies -> https://leetcode.com
#      Firefox: Storage tab -> Cookies -> https://leetcode.com
#      Safari:  Storage tab -> Cookies -> leetcode.com
#   4. Find the row named LEETCODE_SESSION and copy its Value (a long string starting
#      with "eyJ"; double-click the Value cell, Cmd+A, Cmd+C).
#   5. Paste it on the line below (replace the old value), save.
#   6. Check: python3 scripts/leetcode.py login --check   -> "signed in as <you>"
# It expires after a while (or when you log out of leetcode.com); repeat when `attempt`
# warns "scripts/.session is expired". `python3 scripts/leetcode.py login` does steps
# 1-5 in a browser window instead (needs: pip install playwright && playwright install chromium).
"""


def read_session():
    if SESSION_FILE.exists():
        for line in SESSION_FILE.read_text().splitlines():
            line = line.strip()
            if line and not line.startswith("#"):
                return line
    return None


def write_session(session):
    SESSION_FILE.write_text(SESSION_HELP + session.strip() + "\n")


def signed_in_user(session):
    """Return the username the session belongs to, or None when it is missing/expired."""
    if not session:
        return None
    try:
        status = graphql("query{userStatus{isSignedIn username}}", session=session)["userStatus"]
    except (urllib.error.URLError, RuntimeError, KeyError, TimeoutError):
        return None
    return status["username"] if status and status.get("isSignedIn") else None


def login_via_browser():
    """Open Chromium, wait for the user to log in, save LEETCODE_SESSION to scripts/.session."""
    try:
        from playwright.sync_api import TimeoutError as PWTimeout
        from playwright.sync_api import sync_playwright
    except ImportError:
        sys.exit(
            "playwright is not installed: pip install playwright && playwright install chromium\n"
            "or paste the LEETCODE_SESSION cookie value into scripts/.session by hand"
        )
    print("Opening browser - log in to LeetCode; the window closes on its own (3 minutes).")
    with sync_playwright() as pw:
        browser = pw.chromium.launch(headless=False, slow_mo=50)
        ctx = browser.new_context()
        page = ctx.new_page()
        page.goto("https://leetcode.com/accounts/login/")
        try:
            page.wait_for_url(lambda url: "leetcode.com/accounts/login" not in url, timeout=180_000)
        except PWTimeout:
            browser.close()
            sys.exit("timed out waiting for login")
        page.wait_for_load_state("networkidle")
        cookies = {c["name"]: c["value"] for c in ctx.cookies()}
        browser.close()
    session = cookies.get("LEETCODE_SESSION")
    if not session:
        sys.exit("login failed: LEETCODE_SESSION cookie not found")
    write_session(session)
    print(f"session saved to {rel(SESSION_FILE)}")
    return session


def require_session(force_login=False):
    session = None if force_login else read_session()
    if signed_in_user(session):
        return session
    if session:
        warn("scripts/.session is expired")
    return login_via_browser()


_PROBLEM_LIST = None


def problem_list():
    global _PROBLEM_LIST
    if _PROBLEM_LIST is None:
        _PROBLEM_LIST = http_json(LC_ALL_URL)["stat_status_pairs"]
    return _PROBLEM_LIST


def resolve_slug(problem_id):
    for p in problem_list():
        if p["stat"]["frontend_question_id"] == int(problem_id):
            return p["stat"]["question__title_slug"]
    sys.exit(f"error: LeetCode problem {problem_id} not found in the problem list")


def fetch_question(slug):
    query = (
        "query q($titleSlug: String!){question(titleSlug:$titleSlug)"
        "{questionFrontendId title titleSlug content isPaidOnly codeSnippets{langSlug code}}}"
    )
    q = graphql(query, {"titleSlug": slug})["question"]
    if not q or not q.get("content"):
        sys.exit(f"error: no statement returned for '{slug}' (premium-only problem?)")
    return q


# =============================================================================================
# pull: statement -> comment block
# =============================================================================================


class StatementText(HTMLParser):
    """HTML -> plain text that keeps LeetCode's wording and example layout.

    Top-level paragraphs, lists and example blocks are separated by a blank line; lines inside an
    example block or a list item stay compact, as they render on the site. <sup>/<sub> become
    ^ / _, images are dropped (nothing else is).
    """

    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.out = []
        self.pre = 0
        self.lists = []  # stack of [kind, counter]
        self.divs = []  # stack of "is example block" flags
        self.li = 0

    def tail(self):
        return "".join(self.out[-2:])

    def newline(self):
        if self.out and not self.tail().endswith("\n"):
            self.out.append("\n")

    def blank(self):
        self.newline()
        if self.out and not self.tail().endswith("\n\n"):
            self.out.append("\n")

    def compact(self):
        return self.li > 0 or any(self.divs)

    def handle_starttag(self, tag, attrs):
        if tag == "pre":
            self.newline()
            self.pre += 1
        elif tag in ("ul", "ol"):
            self.newline()
            self.lists.append([tag, 0])
        elif tag == "li":
            self.newline()
            self.li += 1
            depth = max(len(self.lists) - 1, 0)
            if self.lists and self.lists[-1][0] == "ol":
                self.lists[-1][1] += 1
                bullet = f"{self.lists[-1][1]}. "
            else:
                bullet = "- "
            self.out.append("  " * depth + bullet)
        elif tag == "div":
            self.newline()
            self.divs.append("example-block" in (dict(attrs).get("class") or ""))
        elif tag == "br":
            self.out.append("\n")
        elif tag == "sup":
            self.out.append("^")
        elif tag == "sub":
            self.out.append("_")
        elif tag in ("td", "th"):
            self.out.append(" | ")
        elif tag in ("p", "table", "tr", "h1", "h2", "h3", "h4", "blockquote"):
            self.newline()
        # img: dropped; inline tags (code, strong, em, span, font, ...): text kept, tag dropped

    def handle_endtag(self, tag):
        if tag == "pre":
            self.pre -= 1
            self.blank()
        elif tag in ("ul", "ol"):
            if self.lists:
                self.lists.pop()
            self.newline() if self.compact() else self.blank()
        elif tag == "li":
            self.li -= 1
            self.newline()
        elif tag == "div":
            was_example = self.divs.pop() if self.divs else False
            self.blank() if was_example else self.newline()
        elif tag == "p":
            self.newline() if self.compact() else self.blank()
        elif tag in ("table", "tr", "h1", "h2", "h3", "h4", "blockquote"):
            self.newline()

    def handle_data(self, data):
        data = data.replace("\xa0", " ")
        if not self.pre:
            # Outside <pre>, source newlines are just HTML formatting.
            data = re.sub(r"\s*\n\s*", " ", data)
            if not self.out or self.tail().endswith("\n") or self.tail().endswith("- "):
                data = data.lstrip()
        if data:
            self.out.append(data)

    def text(self):
        lines = [ln.rstrip() for ln in "".join(self.out).split("\n")]
        text = re.sub(r"\n{3,}", "\n\n", "\n".join(lines)).strip("\n")
        return html.unescape(text)


def statement_block(q, ext):
    parser = StatementText()
    parser.feed(q["content"])
    lines = [
        f"LC {q['questionFrontendId']} - {q['title'].strip()}",
        f"https://leetcode.com/problems/{q['titleSlug']}/",
        "",
    ] + parser.text().split("\n")
    if ext == ".py":
        open_, close = '"""', '"""'
        lines = [ln.replace('"""', '\\"\\"\\"') for ln in lines]
    else:
        open_, close = "/*", "*/"
        lines = [ln.replace("*/", "* /") for ln in lines]
    inner = "\n".join((INDENT + ln) if ln else "" for ln in lines)
    return f"{open_}\n{inner}\n{close}"


def find_solution_file(problem_id):
    pat = re.compile(rf"^0*{int(problem_id)}\. ")
    hits = []
    for root in SEARCH_ROOTS:
        if root.exists():
            hits += [p for p in root.rglob("*") if p.is_file() and pat.match(p.name)]
    hits = [p for p in hits if p.suffix in SOLUTION_EXTS]
    if len(hits) == 1:
        return hits[0]
    if not hits:
        sys.exit(f"error: no file named '{problem_id}. *' found; pass --file <path>")
    listing = "\n".join(f"  {rel(p)}" for p in sorted(hits))
    sys.exit(f"error: several files match '{problem_id}. *', pass --file <path>:\n{listing}")


def comment_re(ext):
    return re.compile(r'"""(.*?)"""', re.S) if ext == ".py" else re.compile(r"/\*(.*?)\*/", re.S)


def place_block(src, block, pid, ext):
    """Insert/replace the statement block; return (new source, what happened)."""
    comment = comment_re(ext)
    for m in comment.finditer(src):  # 1. an earlier pulled block for this id
        if re.match(rf"\s*LC 0*{pid} - ", m.group(1)):
            old = m.group(1)
            # keep attempt metadata recorded in the old block (@status/@time/@wa/@created)
            meta = re.findall(r"^\s*@(?:status|time|wa|created)\s*:.*$", old, re.M)
            if meta:
                url_end = block.index("\n", block.index("https://leetcode.com/problems/")) + 1
                block = block[:url_end] + "\n".join(meta) + "\n" + block[url_end:]
            return src[: m.start()] + block + src[m.end():], "replaced existing statement block"
    for m in comment.finditer(src):  # 2. the first empty placeholder
        if not m.group(1).strip():
            return src[: m.start()] + block + src[m.end():], "filled empty comment placeholder"
    anchor = None  # 3. after `using namespace std;` / last include or import
    for m in re.finditer(r"^(using namespace std;|#include\s.*|import\s.*|from\s.*import.*)$", src, re.M):
        anchor = m
        if m.group(1).startswith("using namespace"):
            break
    if anchor is None:
        return block + "\n\n" + src, "inserted at top of file"
    pos = anchor.end()
    rest = src[pos:].lstrip("\n")
    return src[:pos] + "\n\n" + block + "\n" + rest, f"inserted after `{anchor.group(1)}`"


def cpp_snippet(q):
    return next((s["code"] for s in q.get("codeSnippets") or [] if s["langSlug"] == "cpp"), None)


def method_mismatch(q, src):
    snippet = cpp_snippet(q)
    if not snippet:
        return None
    names = re.findall(r"\b(\w+)\s*\([^()]*\)\s*\{", snippet)
    names = [n for n in names if n not in ("Solution", "if", "for", "while", "switch")]
    if names and not any(re.search(rf"\b{n}\s*\(", src) for n in names):
        return (
            f"warning: LeetCode's C++ method `{names[0]}` does not appear in the file - the code "
            f"may solve a different problem than LC {q['questionFrontendId']}"
        )
    return None


def cmd_pull(args):
    path = (args.file if args.file else find_solution_file(args.id)).resolve()
    q = fetch_question(resolve_slug(args.id))
    block = statement_block(q, path.suffix)
    if args.dry_run:
        print(block)
        return
    src = path.read_text()
    new, how = place_block(src, block, args.id, path.suffix)
    path.write_text(new)
    print(f"{rel(path)}: {how} (LC {q['questionFrontendId']} - {q['title'].strip()})")
    mismatch = method_mismatch(q, src)
    if mismatch:
        print(mismatch)


# =============================================================================================
# attempt: @created / @time / @wa
# =============================================================================================

META_KEYS = ("status", "time", "wa", "created")
STATUSES = ("self", "hint", "editorial", "copied")
CREATED_FMT = "%Y-%m-%d %H:%M"
SUBMISSIONS_QUERY = """
query submissionList($offset: Int!, $limit: Int!, $lastKey: String, $questionSlug: String!) {
  questionSubmissionList(offset: $offset, limit: $limit, lastKey: $lastKey, questionSlug: $questionSlug) {
    lastKey
    hasNext
    submissions { id statusDisplay lang timestamp }
  }
}
"""


def statement_comment(src, ext):
    """The (match, LC id, slug) of the pulled `LC <id> - ...` block, or None."""
    for m in comment_re(ext).finditer(src):
        head = re.match(r"\s*LC\s+(\d+)\s*-", m.group(1))
        if head:
            slug = re.search(r"https://leetcode\.com/problems/([\w-]+)/", m.group(1))
            return m, int(head.group(1)), slug.group(1) if slug else None
    return None


def read_meta(block_text):
    meta = {}
    for key, value in re.findall(r"^\s*@(status|time|wa|created)\s*:\s*(.*?)\s*$", block_text, re.M):
        meta.setdefault(key, value)
    return meta


def is_tracked(path):
    res = subprocess.run(
        ["git", "-C", str(REPO), "ls-files", "--error-unmatch", str(path)],
        capture_output=True, text=True, check=False,
    )
    return res.returncode == 0


def fmt_ts(ts):
    return datetime.fromtimestamp(ts).strftime(CREATED_FMT) if ts is not None else None


def parse_created(value):
    try:
        return datetime.strptime(value.strip(), CREATED_FMT).timestamp()
    except (ValueError, AttributeError):
        return None


def submissions(slug, session):
    """All of the account's submissions for a problem, oldest first."""
    subs, offset, last_key = [], 0, None
    while True:
        data = graphql(
            SUBMISSIONS_QUERY,
            {"offset": offset, "limit": 20, "lastKey": last_key, "questionSlug": slug},
            session,
        )["questionSubmissionList"]
        if not data:
            break
        subs += data.get("submissions") or []
        if not data.get("hasNext"):
            break
        offset += 20
        last_key = data.get("lastKey")
        time.sleep(0.3)
    for s in subs:
        s["timestamp"] = int(s["timestamp"])
    return sorted(subs, key=lambda s: s["timestamp"])


def analyze_attempt(path, session):
    """Compute the attempt metadata of one solution file (nothing is written)."""
    path = Path(path).resolve()
    src = path.read_text(encoding="utf-8", errors="replace")
    found = statement_comment(src, path.suffix)
    info = {"file": str(rel(path)), "lc_id": None, "slug": None, "notes": []}
    if not found:
        info["notes"].append("no pulled `LC <id> - ...` block; run `pull` first")
        return info
    m, lc_id, slug = found
    info.update(lc_id=lc_id, slug=slug or resolve_slug(lc_id))
    meta = read_meta(m.group(1))
    info["recorded"] = meta

    # --- start: @created already recorded > file birth time (only trustworthy while untracked)
    created = parse_created(meta.get("created"))
    if created is not None:
        info["created_source"] = "@created line"
    elif is_tracked(path):
        info["notes"].append("file is tracked by git: its birth time was reset by checkouts")
    else:
        st = path.stat()
        created = getattr(st, "st_birthtime", None)
        if created is None:
            info["notes"].append("this filesystem does not report a file birth time")
        else:
            info["created_source"] = "file birth time"
    info["created"] = fmt_ts(created)

    # --- end + wrong answers: first Accepted submission at/after @created (older ones = earlier solve)
    ac_ts, wa, rejected = None, None, []
    if session and created is not None:
        try:
            subs = submissions(info["slug"], session)
        except (urllib.error.URLError, RuntimeError, KeyError, TimeoutError) as exc:
            subs = None
            info["notes"].append(f"could not read submissions ({exc})")
        if subs is not None:
            window = [s for s in subs if s["timestamp"] >= created]
            first_ac = next((s for s in window if s["statusDisplay"] == "Accepted"), None)
            if first_ac:
                ac_ts = first_ac["timestamp"]
                rejected = [s["statusDisplay"] for s in window if s["timestamp"] < ac_ts]
                wa = len(rejected)
                info["end_source"] = "first Accepted submission after @created"
            else:
                before = sum(s["statusDisplay"] == "Accepted" for s in subs if s["timestamp"] < created)
                note = "no Accepted submission after @created"
                if before:
                    note += f" ({before} older Accepted ignored: an earlier solve)"
                info["notes"].append(note)
    elif not session:
        info["notes"].append("no valid scripts/.session: @wa cannot be read from LeetCode")

    end = ac_ts
    if end is None and created is not None and not is_tracked(path):
        end = path.stat().st_mtime
        info["end_source"] = "last edit of the file (no Accepted found)"
    info["accepted"] = fmt_ts(ac_ts)
    info["end"] = fmt_ts(end)
    info["wa"] = wa
    info["rejected"] = rejected
    info["minutes"] = (
        max(1, math.ceil((end - created) / 60)) if created is not None and end is not None and end >= created else None
    )
    return info


def write_meta(path, values):
    """Set @status/@time/@wa/@created lines (in that order) right under the statement URL."""
    path = Path(path).resolve()
    src = path.read_text(encoding="utf-8")
    found = statement_comment(src, path.suffix)
    if not found:
        sys.exit(f"error: {rel(path)} has no pulled `LC <id> - ...` block; run `pull` first")
    m = found[0]
    body = m.group(1)
    meta = read_meta(body)
    meta.update({k: str(v) for k, v in values.items() if v is not None})
    body = re.sub(r"^[ \t]*@(?:status|time|wa|created)\s*:.*\n", "", body, flags=re.M)
    url = re.search(r"^.*https://leetcode\.com/problems/[\w-]+/.*\n", body, re.M)
    if not url:
        sys.exit(f"error: no problem URL line in the statement block of {rel(path)}")
    lines = "".join(f"{INDENT}@{k}: {meta[k]}\n" for k in META_KEYS if meta.get(k))
    body = body[: url.end()] + lines + body[url.end():]
    start = m.start(1)
    path.write_text(src[:start] + body + src[m.end(1):], encoding="utf-8")
    return {k: meta[k] for k in META_KEYS if meta.get(k)}


def cmd_attempt(args):
    session = read_session()
    if not signed_in_user(session):
        if session:
            warn("scripts/.session is expired - run `python3 scripts/leetcode.py login`")
        session = None
    if args.write:
        if len(args.files) != 1:
            sys.exit("error: --write takes exactly one file")
        if args.status and args.status not in STATUSES:
            sys.exit(f"error: --status must be one of {', '.join(STATUSES)}")
        info = analyze_attempt(args.files[0], session)
        values = {
            "status": args.status,
            "time": args.time if args.time is not None else info.get("minutes"),
            "wa": args.wa if args.wa is not None else info.get("wa"),
            "created": info.get("created"),
        }
        written = write_meta(args.files[0], values)
        print(f"{info['file']}: " + "  ".join(f"@{k}: {v}" for k, v in written.items()))
        return
    infos = [analyze_attempt(f, session) for f in args.files]
    if args.json:
        print(json.dumps(infos, ensure_ascii=False, indent=1))
        return
    for i in infos:
        print(f"{i['file']}  (LC {i['lc_id']})")
        print(f"  created : {i.get('created') or '?'}  [{i.get('created_source', 'unknown')}]")
        print(f"  end     : {i.get('end') or '?'}  [{i.get('end_source', 'unknown')}]")
        mins = i.get("minutes")
        print(f"  @time   : {mins if mins is not None else '?'} min" + (f"  (~{mins / 1440:.1f} days)" if mins and mins >= 1440 else ""))
        wa = i.get("wa")
        print(f"  @wa     : {wa if wa is not None else '?'}" + (f"  ({', '.join(i['rejected'])})" if i.get("rejected") else ""))
        if i.get("recorded"):
            print("  recorded: " + "  ".join(f"@{k}: {v}" for k, v in i["recorded"].items()))
        for n in i["notes"]:
            print(f"  note    : {n}")


# =============================================================================================
# daily: scaffold daily challenge folders
# =============================================================================================

DAILY_QUERY = """
query d($year: Int!, $month: Int!) {
  dailyCodingChallengeV2(year: $year, month: $month) {
    challenges { date question { questionFrontendId title titleSlug } }
  }
}
"""
CPP_HEADER = """#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <queue>
#include <algorithm>
#include <climits>
#include <tuple>
#include <functional>

using namespace std;

"""
CPP_FOOTER = """
auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    return 0;
}
"""


def reindent_snippet(code):
    """LeetCode's 4-space snippet -> the repo's style (`public:` at 4, members at 8)."""
    out = []
    for line in code.rstrip().split("\n"):
        stripped = line.lstrip(" ")
        depth = len(line) - len(stripped)
        if stripped in ("public:", "private:", "protected:"):
            out.append(INDENT + stripped)
        elif depth >= 4 and stripped:
            out.append(" " * (depth + 4) + stripped)
        else:
            out.append(line.rstrip())
    return "\n".join(out) + "\n"


def daily_challenges(day_list):
    by_date = {}
    for year, month in sorted({(d.year, d.month) for d in day_list}):
        data = graphql(DAILY_QUERY, {"year": year, "month": month})["dailyCodingChallengeV2"]
        for c in data["challenges"]:
            by_date[c["date"]] = c["question"]
    return by_date


def cmd_daily(args):
    days = [date.fromisoformat(d) for d in args.dates]
    if args.from_date:
        start = date.fromisoformat(args.from_date)
        end = date.fromisoformat(args.to_date) if args.to_date else date.today()
        days += [start + timedelta(n) for n in range((end - start).days + 1)]
    days = sorted(set(days or [date.today()]))
    challenges = daily_challenges(days)
    for d in days:
        q = challenges.get(d.isoformat())
        if not q:
            warn(f"{d}: no daily challenge published")
            continue
        pid = int(q["questionFrontendId"])
        folder = DAILY_ROOT / d.strftime("%Y_%m_%d")
        existing = [p for p in folder.glob("*") if re.match(rf"^0*{pid}\. ", p.name)] if folder.exists() else []
        if existing:
            print(f"{rel(existing[0])}: already exists, left untouched")
            continue
        others = [p.name for p in folder.glob("*")] if folder.exists() else []
        full = fetch_question(q["titleSlug"])
        snippet = cpp_snippet(full)
        if not snippet:
            warn(f"{d}: LC {pid} has no C++ snippet; skipped")
            continue
        title = re.sub(r'[/\\:*?"<>|]', "-", full["title"].strip())
        path = folder / f"{pid:04d}. {title}.cpp"
        folder.mkdir(parents=True, exist_ok=True)
        src = CPP_HEADER + reindent_snippet(snippet) + CPP_FOOTER
        src, _ = place_block(src, statement_block(full, ".cpp"), pid, ".cpp")
        path.write_text(src)
        print(f"{rel(path)}: created ({d}, LC {pid} - {full['title'].strip()})")
        if others:
            warn(f"{rel(folder)} also holds {', '.join(others)}")


# =============================================================================================
# unsolved: problems you have not solved yet, by topic
# =============================================================================================

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


def fetch_by_tag(tag_slug, difficulty, session):
    variables = {"categorySlug": "", "skip": 0, "limit": 100,
                 "filters": {"tags": [tag_slug], "difficulty": difficulty}}
    data = graphql(PROBLEM_LIST_QUERY, variables, session)
    return (data.get("problemsetQuestionList") or {}).get("questions", [])


_KEYWORD_CACHE = {}


def fetch_by_keyword(keyword, difficulty, session):
    """All results for a keyword (API relevance order), filtered by difficulty."""
    if keyword not in _KEYWORD_CACHE:
        page_size, skip, results = 50, 0, []
        while True:
            variables = {"categorySlug": "", "skip": skip, "limit": page_size,
                         "filters": {"searchKeywords": keyword}}
            data = graphql(PROBLEM_LIST_QUERY, variables, session).get("problemsetQuestionList") or {}
            page = data.get("questions", [])
            results += page
            skip += page_size
            if skip >= data.get("total", 0) or not page:
                break
        _KEYWORD_CACHE[keyword] = results
    return [p for p in _KEYWORD_CACHE[keyword] if p.get("difficulty", "").upper() == difficulty]


def distribute_quota(tags, total):
    """Split `total` across tags with a random 10-20% variation between quotas."""
    import random

    if len(tags) == 1:
        return {tags[0]: total}
    max_var = random.uniform(0.10, 0.20)
    base = total / len(tags)
    quotas, remaining = {}, total
    for tag in tags[:-1]:
        quotas[tag] = max(1, round(base + random.uniform(-max_var, max_var) * base))
        remaining -= quotas[tag]
    quotas[tags[-1]] = max(1, remaining)
    return quotas


def collect_unsolved(tag_slugs, session, target=20, medium_ratio=0.67):
    quotas = distribute_quota(tag_slugs, target)
    print(f"  quota per tag: { {t: quotas[t] for t in tag_slugs} }")
    seen_ids, results, leftovers, deficit = set(), [], [], 0
    for tag in tag_slugs:
        quota = quotas[tag]
        is_keyword = tag.startswith("keyword:")
        pools = {"MEDIUM": [], "HARD": []}
        for diff in ("MEDIUM", "HARD"):
            print(f"  [{diff}] {tag} ...", flush=True)
            try:
                problems = (fetch_by_keyword(tag[len("keyword:"):], diff, session) if is_keyword
                            else fetch_by_tag(tag, diff, session))
            except (urllib.error.URLError, RuntimeError, TimeoutError) as exc:
                print(f"  ERROR: {exc}", file=sys.stderr)
                continue
            for p in problems:
                if p["frontendQuestionId"] in seen_ids or p.get("isPaidOnly"):
                    continue
                if p.get("status") != "ac":
                    pools[diff].append(p)
        if not is_keyword:  # tag search: stable id order; keyword search keeps relevance order
            for diff in pools:
                pools[diff].sort(key=lambda q: int(q["frontendQuestionId"]))
        n_take = min(quota, len(pools["MEDIUM"]) + len(pools["HARD"]))
        n_med = min(round(quota * medium_ratio), len(pools["MEDIUM"]))
        n_hard = min(quota - n_med, len(pools["HARD"]))
        if n_med + n_hard < n_take:
            n_med = min(n_take - n_hard, len(pools["MEDIUM"]))
        selected = pools["MEDIUM"][:n_med] + pools["HARD"][:n_hard]
        for p in selected:
            seen_ids.add(p["frontendQuestionId"])
            results.append(p)
        deficit += quota - len(selected)
        used = {p["frontendQuestionId"] for p in selected}
        leftovers += [p for diff in ("MEDIUM", "HARD") for p in pools[diff]
                      if p["frontendQuestionId"] not in used]
    if deficit > 0:
        unique, seen_lt = [], set()
        for p in leftovers:
            pid = p["frontendQuestionId"]
            if pid not in seen_ids and pid not in seen_lt:
                seen_lt.add(pid)
                unique.append(p)
        med = [p for p in unique if p["difficulty"] == "Medium"]
        hard = [p for p in unique if p["difficulty"] == "Hard"]
        n_med = min(round(deficit * medium_ratio), len(med))
        n_hard = min(deficit - n_med, len(hard))
        results += med[:n_med] + hard[:n_hard]
    results.sort(key=lambda q: int(q["frontendQuestionId"]))
    return results[:target]


def cmd_unsolved(args):
    session = require_session(force_login=args.relogin)
    tag_slugs = [TOPIC_ALIASES.get(t.lower(), t) for t in args.topics]
    print(f"\nsearching {args.count} unsolved problems: {tag_slugs}")
    problems = collect_unsolved(tag_slugs, session, target=args.count)
    if not problems:
        print("no unsolved problems found")
        return
    print(f"\n{'#':<5} {'ID':<6} {'Diff':<8} {'AC%':>6}  {'Title':<50}  URL")
    print("-" * 120)
    for i, p in enumerate(problems, 1):
        ac = f"{p['acRate']:.1f}%" if p.get("acRate") else "  n/a"
        url = f"https://leetcode.com/problems/{p['titleSlug']}/"
        print(f"{i:<5} {p['frontendQuestionId']:<6} {p['difficulty']:<8} {ac:>6}  {p['title']:<50}  {url}")


# =============================================================================================
# feed: JSON feed for the Omnia "cultivation" Anki plugin
# =============================================================================================
#
# The Anki add-on cannot run git, so this COLLECTS the data it needs and nothing else; all rating
# math (Elo, neglect decay, realms, trials) lives in the add-on.
#
# Feed schema 1:
# - `attempts`: one entry per LeetCode solution file under src/leetcode/{daily,problems,contests}/
#   with its LC id, date, source, topic and the @status/@time/@wa/@created metadata of the first
#   block comment (no @status -> "unknown").
#     - LC id: the `LC <id> - ...` header of the pulled statement if present, else the filename's
#       leading integer (daily/ and problems/). Contest files are named by question index
#       (`1.cpp`, `Q1. Title.cpp`), so they resolve through zerotrac's (contest, Q-index) table or
#       the title; a file that cannot be resolved is skipped with a warning, never guessed.
#     - date: the folder date for daily/, else the file's first-commit date; uncommitted -> today.
# - `problems`: zerotrac's contest-rated problems (Elo scale), used for ratings and trial picks.
# - `difficulty`: LeetCode's Easy/Medium/Hard per problem id, the fallback for unrated problems.
# Both remote lists are cached under build/cultivation-cache/ and refreshed after 7 days. Any
# network failure falls back to the cache (or to empty lists) with a warning: the feed must never
# break the commit flow. Output is written atomically (temp file + rename).

FEED_SCHEMA = 1
FEED_SOURCES = ("daily", "problems", "contests")
FEED_EXTENSIONS = {".cpp", ".cc", ".py", ".java", ".go", ".js", ".ts", ".rs", ".kt"}
ZEROTRAC_URL = "https://raw.githubusercontent.com/zerotrac/leetcode_problem_rating/main/data.json"
CACHE_MAX_AGE_S = 7 * 86400
DIFFICULTY = {1: "Easy", 2: "Medium", 3: "Hard"}

BLOCK_RE = re.compile(r"/\*(.*?)\*/|\"\"\"(.*?)\"\"\"", re.S)
LC_HEADER_RE = re.compile(r"^\s*LC\s+(\d+)\s*-\s*(.+?)\s*$", re.M)
FEED_META_RE = re.compile(r"^\s*@(status|time|wa)\s*:\s*([^\s/#]+)", re.M | re.I)
FEED_CREATED_RE = re.compile(r"^\s*@created\s*:\s*(\d{4}-\d{2}-\d{2}[ T]\d{2}:\d{2})", re.M | re.I)
LEADING_ID_RE = re.compile(r"^0*(\d+)(?:[._\s-]|$)")
CONTEST_FILE_RE = re.compile(r"^Q?(\d)(?:\.\s*(.+))?$", re.I)
DAILY_DIR_RE = re.compile(r"^(\d{4})_(\d{2})_(\d{2})$")


def cached_json(cache_file, url, offline):
    """Return (data, age_days) from the cache, refreshing it when stale and online."""
    fresh = cache_file.exists() and time.time() - cache_file.stat().st_mtime < CACHE_MAX_AGE_S
    if not fresh and not offline:
        try:
            data = http_json(url)
            cache_file.parent.mkdir(parents=True, exist_ok=True)
            cache_file.write_text(json.dumps(data), encoding="utf-8")
            return data, 0.0
        except Exception as exc:  # noqa: BLE001 - any fetch failure falls back to the cache
            warn(f"could not fetch {url} ({exc}); using the cached copy if any")
    if cache_file.exists():
        age = (time.time() - cache_file.stat().st_mtime) / 86400
        return json.loads(cache_file.read_text(encoding="utf-8")), age
    warn(f"no cached copy of {url}; the feed will lack that list")
    return None, None


def load_rated_problems(raw):
    out = []
    for row in raw or []:
        try:
            out.append({
                "id": int(row["ID"]),
                "title": row.get("Title"),
                "slug": row.get("TitleSlug"),
                "rating": round(float(row["Rating"]), 2),
                "contest": row.get("ContestSlug"),
                "index": row.get("ProblemIndex"),
            })
        except (KeyError, TypeError, ValueError):
            continue
    return out


def norm_title(title):
    return re.sub(r"[^a-z0-9]+", " ", title.lower()).strip()


def load_difficulty(raw):
    """LeetCode's all-problems list -> ({id: "Easy|Medium|Hard"}, {normalized title: id})."""
    difficulty, by_title = {}, {}
    for pair in (raw or {}).get("stat_status_pairs", []):
        try:
            pid = int(pair["stat"]["frontend_question_id"])
        except (KeyError, TypeError, ValueError):
            continue
        level = DIFFICULTY.get((pair.get("difficulty") or {}).get("level"))
        if level:
            difficulty[str(pid)] = level
        title = pair["stat"].get("question__title")
        if title:
            by_title[norm_title(title)] = pid
    return difficulty, by_title


def git(repo, *args):
    try:
        res = subprocess.run(["git", "-C", str(repo), *args], capture_output=True, text=True, check=False)
    except OSError:
        return ""
    return res.stdout.strip() if res.returncode == 0 else ""


def first_commit_dates(repo):
    """repo-relative path -> date the file was first created, following renames (one pass)."""
    out = git(repo, "log", "--reverse", "-M", "--name-status", "--format=@@%aI")
    dates, current = {}, None
    for line in out.splitlines():
        if line.startswith("@@"):
            current = line[2:12]
            continue
        parts = line.split("\t")
        if not current or len(parts) < 2:
            continue
        status = parts[0][:1]
        if status == "A":
            dates.setdefault(parts[1], current)
        elif status in ("R", "C") and len(parts) >= 3:
            dates.setdefault(parts[2], dates.get(parts[1], current))
    return dates


def to_int(value):
    try:
        return int(value) if value is not None else None
    except ValueError:
        return None


def parse_feed_meta(text):
    """Return (lc_id, title, status, minutes, wa, created) from the first block holding them."""
    lc_id = title = created = None
    meta = {}
    for match in BLOCK_RE.finditer(text):
        block = match.group(1) or match.group(2) or ""
        header = LC_HEADER_RE.search(block)
        if header and lc_id is None:
            lc_id, title = int(header.group(1)), header.group(2)
        for key, value in FEED_META_RE.findall(block):
            meta.setdefault(key.lower(), value)
        c = FEED_CREATED_RE.search(block)
        if c and created is None:
            created = c.group(1).replace("T", " ")
        if header or meta:
            break
    status = (meta.get("status") or "").lower()
    if status not in STATUSES:
        if status:
            warn(f"unknown @status {status!r}; treated as unknown")
        status = "unknown"
    return lc_id, title, status, to_int(meta.get("time")), to_int(meta.get("wa")), created


def contest_lookup(problems):
    """(contest number, question index 1-4) -> problem, from zerotrac's weekly/biweekly slugs."""
    table = {}
    for p in problems:
        m = re.match(r"^(?:bi)?weekly-contest-(\d+)$", p.get("contest") or "")
        q = re.match(r"^Q(\d)$", p.get("index") or "")
        if m and q:
            table.setdefault((int(m.group(1)), int(q.group(1))), p)
    return table


def resolve_contest(path, title_hint, by_contest, by_title):
    m = CONTEST_FILE_RE.match(path.stem)
    if title_hint is None and m and m.group(2):
        title_hint = m.group(2)
    if title_hint and norm_title(title_hint) in by_title:
        return by_title[norm_title(title_hint)], title_hint
    if m and path.parent.name.isdigit():
        hit = by_contest.get((int(path.parent.name), int(m.group(1))))
        if hit:
            return hit["id"], hit["title"]
    return None


def collect_feed_attempts(repo, by_contest, by_title):
    root = repo / "src" / "leetcode"
    committed = first_commit_dates(repo)
    today = date.today().isoformat()
    attempts, skipped = [], []
    for source in FEED_SOURCES:
        base = root / source
        if not base.is_dir():
            continue
        for path in sorted(base.rglob("*")):
            if not path.is_file() or path.suffix not in FEED_EXTENSIONS:
                continue
            relpath = path.relative_to(repo).as_posix()
            text = path.read_text(encoding="utf-8", errors="replace")
            lc_id, title, status, minutes, wa, created = parse_feed_meta(text)
            if source == "contests":
                resolved = (lc_id, title) if lc_id else resolve_contest(path, title, by_contest, by_title)
                if not resolved:
                    skipped.append(relpath)
                    continue
                lc_id, title = resolved
            elif lc_id is None:
                m = LEADING_ID_RE.match(path.stem)
                if not m:
                    skipped.append(relpath)
                    continue
                lc_id = int(m.group(1))
                rest = path.stem[m.end():].replace("_", " ").strip(" .-")
                title = title or rest or None
            day = None
            if source == "daily":
                dm = DAILY_DIR_RE.match(path.relative_to(base).parts[0])
                if dm:
                    day = f"{dm.group(1)}-{dm.group(2)}-{dm.group(3)}"
            day = day or committed.get(relpath) or today
            attempts.append({
                "lc_id": lc_id,
                "title": title,
                "path": relpath,
                "source": source,
                "topic": path.relative_to(base).parts[0] if source == "problems" else None,
                "date": day,
                "status": status,
                "minutes": minutes,
                "wa": wa,
                "created": created,
            })
    attempts.sort(key=lambda a: (a["date"], a["path"]))
    return attempts, skipped


def feed_output_path(arg):
    if arg:
        return Path(arg).expanduser()
    env = os.environ.get("OMNIA_CULTIVATION_FEED")
    if env:
        return Path(env).expanduser()
    return Path.home() / ".omnia" / "cultivation" / "dsa_feed.json"


def write_atomic(path, payload):
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, tmp = tempfile.mkstemp(dir=path.parent, prefix=".dsa_feed.", suffix=".tmp")
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as fh:
            json.dump(payload, fh, ensure_ascii=False, indent=1)
        os.replace(tmp, path)
    except BaseException:
        Path(tmp).unlink(missing_ok=True)
        raise


def cmd_feed(args):
    repo = Path(args.repo).expanduser().resolve()
    cache = repo / "build" / "cultivation-cache"
    raw_problems, ratings_age = cached_json(cache / "zerotrac.json", ZEROTRAC_URL, args.offline)
    raw_all, _ = cached_json(cache / "leetcode_all.json", LC_ALL_URL, args.offline)
    problems = load_rated_problems(raw_problems)
    difficulty, by_title = load_difficulty(raw_all)
    for p in problems:  # zerotrac titles also resolve contest files when LeetCode's list is absent
        by_title.setdefault(norm_title(p["title"] or ""), p["id"])
    attempts, skipped = collect_feed_attempts(repo, contest_lookup(problems), by_title)
    for relpath in skipped:
        warn(f"skipped {relpath}: could not resolve its LeetCode id")
    feed = {
        "schema": FEED_SCHEMA,
        "generated_at": datetime.now().astimezone().isoformat(timespec="seconds"),
        "repo": str(repo),
        "commit": git(repo, "rev-parse", "HEAD") or None,
        "attempts": attempts,
        "problems": problems,
        "difficulty": difficulty,
    }
    out = feed_output_path(args.out)
    tagged = sum(a["status"] != "unknown" for a in attempts)
    age = "none" if ratings_age is None else f"{ratings_age:.1f}d"
    summary = (
        f"cultivation feed: {len(attempts)} attempts ({tagged} with @status, "
        f"{len(attempts) - tagged} unknown, {len(skipped)} skipped), "
        f"{len(problems)} rated problems (cache age {age})"
    )
    if args.dry_run:
        for a in attempts[-5:]:
            print(json.dumps(a, ensure_ascii=False))
        print(f"{summary} -> dry run, nothing written")
        return
    write_atomic(out, feed)
    print(f"{summary} -> {out}")


# =============================================================================================
# CLI
# =============================================================================================


def cmd_login(args):
    if args.check:
        session = read_session()
        user = signed_in_user(session)
        if user:
            print(f"signed in as {user}")
            return
        print("scripts/.session is " + ("expired" if session else "missing")
              + " - see the instructions at the top of scripts/.session")
        if not SESSION_FILE.exists() or session is None:
            write_session("")  # leave the instructions in place for the user
        sys.exit(1)
    session = login_via_browser()
    print(f"signed in as {signed_in_user(session) or '?'}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("pull", help="pull a statement verbatim into its solution file")
    p.add_argument("id", type=int)
    p.add_argument("--file", type=Path, help="target file (needed when several or no files match)")
    p.add_argument("--dry-run", action="store_true", help="print the block, do not edit")
    p.set_defaults(func=cmd_pull)

    p = sub.add_parser("attempt", help="compute (and record) @created/@time/@wa of solution files")
    p.add_argument("files", nargs="+", type=Path)
    p.add_argument("--json", action="store_true", help="machine-readable output")
    p.add_argument("--write", action="store_true", help="record the metadata in the file")
    p.add_argument("--status", help=f"with --write: one of {', '.join(STATUSES)}")
    p.add_argument("--time", type=int, help="with --write: override the computed minutes")
    p.add_argument("--wa", type=int, help="with --write: override the computed wrong submissions")
    p.set_defaults(func=cmd_attempt)

    p = sub.add_parser("daily", help="scaffold daily challenge folders")
    p.add_argument("dates", nargs="*", help="YYYY-MM-DD (default: today)")
    p.add_argument("--from", dest="from_date", help="first date of a range (YYYY-MM-DD)")
    p.add_argument("--to", dest="to_date", help="last date of the range (default: today)")
    p.set_defaults(func=cmd_daily)

    p = sub.add_parser("unsolved", help="list unsolved problems by topic")
    p.add_argument("--topics", nargs="+", default=["shortest-path", "mst"],
                   help="topics: " + ", ".join(TOPIC_ALIASES))
    p.add_argument("--count", type=int, default=20, help="number of problems (default 20)")
    p.add_argument("--relogin", action="store_true", help="log in again even with a cached session")
    p.set_defaults(func=cmd_unsolved)

    p = sub.add_parser("feed", help="export the Omnia cultivation feed")
    p.add_argument("--repo", default=str(REPO), help="dsa-journey checkout to read")
    p.add_argument("--out", help="feed path (default: $OMNIA_CULTIVATION_FEED or ~/.omnia/...)")
    p.add_argument("--offline", action="store_true", help="use cached lists only")
    p.add_argument("--dry-run", action="store_true", help="print a summary, write nothing")
    p.set_defaults(func=cmd_feed)

    p = sub.add_parser("login", help="refresh scripts/.session through a browser")
    p.add_argument("--check", action="store_true", help="only report whether the session is valid")
    p.set_defaults(func=cmd_login)

    args = ap.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
