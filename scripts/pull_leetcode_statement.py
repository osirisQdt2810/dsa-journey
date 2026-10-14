#!/usr/bin/env python3
"""Pull the verbatim statement of a LeetCode problem into a comment block of its solution file.

Usage:
    python3 scripts/pull_leetcode_statement.py <ID> [--file <path>] [--dry-run]

- <ID> is the LeetCode frontend number (e.g. 3727). It is resolved to a slug via the public
  problem list, then the statement HTML is fetched from the GraphQL `question` query.
- The target file is the single file under src/leetcode/ or courses/ whose name starts with
  `<ID>. ` (zero-padding allowed, e.g. `0076. ...`). Pass --file when there are several or none.
- The HTML is converted to plain text with the wording kept exactly as on LeetCode: <sup>/<sub>
  become ^ / _, list items become "- ", <pre> example blocks keep their layout. Images are
  dropped (nothing else is).
- Placement, in order of preference:
    1. an existing pulled block for the same id (first line `LC <ID> - ...`) -> replaced in place,
       so re-running is idempotent;
    2. the first empty `/* */` placeholder in the file (the daily template has one above the
       Solution class);
    3. a new block right after `using namespace std;` (or after the last #include / import).
- The solution's C++ method name is compared with LeetCode's C++ code snippet; a mismatch (the
  filename names a different problem than the code solves) is reported as a warning.
"""

import argparse
import html
import json
import re
import sys
import urllib.request
from html.parser import HTMLParser
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
SEARCH_ROOTS = [REPO / "src" / "leetcode", REPO / "courses"]
UA = {"User-Agent": "Mozilla/5.0", "Content-Type": "application/json"}
INDENT = "    "


def http_json(url, payload=None):
    data = json.dumps(payload).encode() if payload is not None else None
    req = urllib.request.Request(url, data=data, headers=UA)
    with urllib.request.urlopen(req, timeout=30) as resp:
        return json.load(resp)


def resolve_slug(problem_id):
    pairs = http_json("https://leetcode.com/api/problems/all/")["stat_status_pairs"]
    for p in pairs:
        if p["stat"]["frontend_question_id"] == problem_id:
            return p["stat"]["question__title_slug"]
    sys.exit(f"error: LeetCode problem {problem_id} not found in the problem list")


def fetch_question(slug):
    query = (
        "query q($titleSlug: String!){question(titleSlug:$titleSlug)"
        "{questionFrontendId title titleSlug content isPaidOnly codeSnippets{langSlug code}}}"
    )
    q = http_json(
        "https://leetcode.com/graphql",
        {"query": query, "variables": {"titleSlug": slug}},
    )["data"]["question"]
    if not q or not q.get("content"):
        sys.exit(f"error: no statement returned for '{slug}' (premium-only problem?)")
    return q


class StatementText(HTMLParser):
    """HTML -> plain text that keeps LeetCode's wording and example layout.

    Top-level paragraphs, lists and example blocks are separated by a blank line; lines inside an
    example block or a list item stay compact, as they render on the site.
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
        text = "\n".join(lines)
        text = re.sub(r"\n{3,}", "\n\n", text).strip("\n")
        return html.unescape(text)


def to_text(content):
    parser = StatementText()
    parser.feed(content)
    return parser.text()


def build_block(q, ext):
    pid, title, slug = q["questionFrontendId"], q["title"].strip(), q["titleSlug"]
    body = to_text(q["content"])
    lines = [f"LC {pid} - {title}", f"https://leetcode.com/problems/{slug}/", ""]
    lines += body.split("\n")
    if ext == ".py":
        open_, close = '"""', '"""'
        lines = [ln.replace('"""', '\\"\\"\\"') for ln in lines]
    else:
        open_, close = "/*", "*/"
        lines = [ln.replace("*/", "* /") for ln in lines]
    inner = "\n".join((INDENT + ln) if ln else "" for ln in lines)
    return f"{open_}\n{inner}\n{close}"


def find_file(problem_id):
    pat = re.compile(rf"^0*{problem_id}\. ")
    hits = []
    for root in SEARCH_ROOTS:
        if root.exists():
            hits += [p for p in root.rglob("*") if p.is_file() and pat.match(p.name)]
    hits = [p for p in hits if p.suffix in (".cpp", ".cc", ".h", ".hpp", ".py")]
    if len(hits) == 1:
        return hits[0]
    if not hits:
        sys.exit(f"error: no file named '{problem_id}. *' found; pass --file <path>")
    listing = "\n".join(f"  {p.relative_to(REPO)}" for p in sorted(hits))
    sys.exit(f"error: several files match '{problem_id}. *', pass --file <path>:\n{listing}")


def place(src, block, pid, ext):
    if ext == ".py":
        comment = re.compile(r'"""(.*?)"""', re.S)
    else:
        comment = re.compile(r"/\*(.*?)\*/", re.S)

    # 1. an earlier pulled block for this id
    for m in comment.finditer(src):
        if re.match(rf"\s*LC 0*{pid} - ", m.group(1)):
            return src[: m.start()] + block + src[m.end():], "replaced existing statement block"
    # 2. the first empty placeholder
    for m in comment.finditer(src):
        if not m.group(1).strip():
            return src[: m.start()] + block + src[m.end():], "filled empty comment placeholder"
    # 3. after `using namespace std;` / last include or import
    anchor = None
    for m in re.finditer(r"^(using namespace std;|#include\s.*|import\s.*|from\s.*import.*)$", src, re.M):
        anchor = m
        if m.group(1).startswith("using namespace"):
            break
    if anchor is None:
        return block + "\n\n" + src, "inserted at top of file"
    pos = anchor.end()
    rest = src[pos:].lstrip("\n")
    return src[:pos] + "\n\n" + block + "\n" + rest, f"inserted after `{anchor.group(1)}`"


def check_method(q, src):
    snippet = next((s["code"] for s in q["codeSnippets"] or [] if s["langSlug"] == "cpp"), None)
    if not snippet:
        return None
    names = re.findall(r"\b(\w+)\s*\([^()]*\)\s*\{", snippet)
    names = [n for n in names if n not in ("Solution", "if", "for", "while", "switch")]
    if names and not any(re.search(rf"\b{n}\s*\(", src) for n in names):
        return f"warning: LeetCode's C++ method `{names[0]}` does not appear in the file — the code may solve a different problem than LC {q['questionFrontendId']}"
    return None


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("id", type=int)
    ap.add_argument("--file", type=Path)
    ap.add_argument("--dry-run", action="store_true", help="print the block, do not edit")
    args = ap.parse_args()

    path = (args.file if args.file else find_file(args.id)).resolve()
    q = fetch_question(resolve_slug(args.id))
    block = build_block(q, path.suffix)
    if args.dry_run:
        print(block)
        return
    src = path.read_text()
    new, how = place(src, block, args.id, path.suffix)
    path.write_text(new)
    shown = path.relative_to(REPO) if path.is_relative_to(REPO) else path
    print(f"{shown}: {how} (LC {q['questionFrontendId']} - {q['title'].strip()})")
    warn = check_method(q, src)
    if warn:
        print(warn)


if __name__ == "__main__":
    main()
