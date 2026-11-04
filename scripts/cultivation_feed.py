#!/usr/bin/env python3
"""Export this repo's LeetCode attempts as a JSON feed for the Omnia "cultivation" plugin.

Usage:
    python3 scripts/cultivation_feed.py [--repo PATH] [--out PATH] [--offline] [--dry-run]

The Anki add-on cannot run git, so this script COLLECTS the data it needs and nothing else; all
rating math (Elo, neglect decay, realms, trials) lives in the add-on. It is run as the last step
of `/commit-progress`, so the feed is fresh after every study commit.

What goes into the feed (schema 1):
- `attempts`: one entry per LeetCode solution file under src/leetcode/{daily,problems,contests}/
  with its LC id, date, source, topic and the `@status` / `@time` / `@wa` metadata from the first
  block comment (absent -> status "unknown").
    - LC id: the `LC <id> - ...` header of the pulled statement if present, else the filename's
      leading integer (daily/ and problems/). Contest files are named by question index
      (`1.cpp`, `Q1. Title.cpp`), so they resolve through zerotrac's (contest, Q-index) table or
      the title; a file that cannot be resolved is skipped with a warning, never guessed.
    - date: the folder date for daily/, else the file's first-commit date; uncommitted -> today.
- `problems`: zerotrac's contest-rated problems (Elo scale), used for ratings and trial picks.
- `difficulty`: LeetCode's Easy/Medium/Hard per problem id, the fallback for unrated problems.

Both remote lists are cached under build/cultivation-cache/ and refreshed after 7 days. Any
network failure falls back to the cache (or to empty lists) with a warning: this script must
never break the commit flow.

Output: --out, else $OMNIA_CULTIVATION_FEED, else ~/.omnia/cultivation/dsa_feed.json. Written
atomically (temp file + rename) so the add-on never reads half a file.
"""

import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
import time
import urllib.request
from datetime import date, datetime
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
SCHEMA = 1
SOURCES = ("daily", "problems", "contests")
EXTENSIONS = {".cpp", ".cc", ".py", ".java", ".go", ".js", ".ts", ".rs", ".kt"}
STATUSES = ("self", "hint", "editorial", "copied")

ZEROTRAC_URL = "https://raw.githubusercontent.com/zerotrac/leetcode_problem_rating/main/data.json"
LC_ALL_URL = "https://leetcode.com/api/problems/all/"
CACHE_MAX_AGE_S = 7 * 86400
# LeetCode answers 403 to a bare "Mozilla/5.0" UA; same headers as pull_leetcode_statement.py.
UA = {
    "User-Agent": (
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 "
        "(KHTML, like Gecko) Chrome/126.0 Safari/537.36"
    ),
    "Referer": "https://leetcode.com/",
    "Content-Type": "application/json",
}
DIFFICULTY = {1: "Easy", 2: "Medium", 3: "Hard"}

BLOCK_RE = re.compile(r"/\*(.*?)\*/|\"\"\"(.*?)\"\"\"", re.S)
LC_HEADER_RE = re.compile(r"^\s*LC\s+(\d+)\s*-\s*(.+?)\s*$", re.M)
META_RE = re.compile(r"^\s*@(status|time|wa)\s*:\s*([^\s/#]+)", re.M | re.I)
LEADING_ID_RE = re.compile(r"^0*(\d+)(?:[._\s-]|$)")
CONTEST_FILE_RE = re.compile(r"^Q?(\d)(?:\.\s*(.+))?$", re.I)
DAILY_DIR_RE = re.compile(r"^(\d{4})_(\d{2})_(\d{2})$")


def warn(msg):
    print(f"warning: {msg}", file=sys.stderr)


# ---------------------------------------------------------------------------------------------
# Remote lists (cached)
# ---------------------------------------------------------------------------------------------


def http_json(url):
    req = urllib.request.Request(url, headers=UA)
    with urllib.request.urlopen(req, timeout=30) as resp:
        return json.load(resp)


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


def load_problems(raw):
    """zerotrac rows -> the feed's `problems` list."""
    out = []
    for row in raw or []:
        try:
            out.append(
                {
                    "id": int(row["ID"]),
                    "title": row.get("Title"),
                    "slug": row.get("TitleSlug"),
                    "rating": round(float(row["Rating"]), 2),
                    "contest": row.get("ContestSlug"),
                    "index": row.get("ProblemIndex"),
                }
            )
        except (KeyError, TypeError, ValueError):
            continue
    return out


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


def norm_title(title):
    return re.sub(r"[^a-z0-9]+", " ", title.lower()).strip()


# ---------------------------------------------------------------------------------------------
# Repo parsing
# ---------------------------------------------------------------------------------------------


def git(repo, *args):
    try:
        res = subprocess.run(
            ["git", "-C", str(repo), *args], capture_output=True, text=True, check=False
        )
    except OSError:
        return ""
    return res.stdout.strip() if res.returncode == 0 else ""


def first_commit_dates(repo):
    """Map repo-relative path -> date of the commit that added it (one git call for all)."""
    out = git(
        repo, "log", "--diff-filter=A", "--name-only", "--format=@@%aI", "--reverse",
        "--", "src/leetcode",
    )
    dates, current = {}, None
    for line in out.splitlines():
        if line.startswith("@@"):
            current = line[2:12]
        elif line and current:
            dates.setdefault(line, current)
    return dates


def parse_meta(text):
    """Return (lc_id, title, status, minutes, wa) from the first block comment holding them."""
    lc_id = title = None
    meta = {}
    for match in BLOCK_RE.finditer(text):
        block = match.group(1) or match.group(2) or ""
        header = LC_HEADER_RE.search(block)
        if header and lc_id is None:
            lc_id, title = int(header.group(1)), header.group(2)
        for key, value in META_RE.findall(block):
            meta.setdefault(key.lower(), value)
        if header or meta:
            break
    status = (meta.get("status") or "").lower()
    if status not in STATUSES:
        if status:
            warn(f"unknown @status {status!r}; treated as unknown")
        status = "unknown"
    return lc_id, title, status, to_int(meta.get("time")), to_int(meta.get("wa"))


def to_int(value):
    try:
        return int(value) if value is not None else None
    except ValueError:
        return None


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
    """Resolve a contest file to (lc_id, title) or None."""
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


def collect_attempts(repo, by_contest, by_title):
    root = repo / "src" / "leetcode"
    committed = first_commit_dates(repo)
    today = date.today().isoformat()
    attempts, skipped = [], []
    for source in SOURCES:
        base = root / source
        if not base.is_dir():
            continue
        for path in sorted(base.rglob("*")):
            if not path.is_file() or path.suffix not in EXTENSIONS:
                continue
            rel = path.relative_to(repo).as_posix()
            text = path.read_text(encoding="utf-8", errors="replace")
            lc_id, title, status, minutes, wa = parse_meta(text)
            if source == "contests":
                resolved = (lc_id, title) if lc_id else resolve_contest(
                    path, title, by_contest, by_title
                )
                if not resolved:
                    skipped.append(rel)
                    continue
                lc_id, title = resolved
            elif lc_id is None:
                m = LEADING_ID_RE.match(path.stem)
                if not m:
                    skipped.append(rel)
                    continue
                lc_id = int(m.group(1))
                rest = path.stem[m.end():].replace("_", " ").strip(" .-")
                title = title or rest or None
            day = None
            if source == "daily":
                dm = DAILY_DIR_RE.match(path.relative_to(base).parts[0])
                if dm:
                    day = f"{dm.group(1)}-{dm.group(2)}-{dm.group(3)}"
            day = day or committed.get(rel) or today
            topic = path.relative_to(base).parts[0] if source == "problems" else None
            attempts.append(
                {
                    "lc_id": lc_id,
                    "title": title,
                    "path": rel,
                    "source": source,
                    "topic": topic,
                    "date": day,
                    "status": status,
                    "minutes": minutes,
                    "wa": wa,
                }
            )
    attempts.sort(key=lambda a: (a["date"], a["path"]))
    return attempts, skipped


# ---------------------------------------------------------------------------------------------
# Output
# ---------------------------------------------------------------------------------------------


def output_path(arg):
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


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--repo", default=str(REPO), help="dsa-journey checkout to read")
    ap.add_argument("--out", help="feed path (default: $OMNIA_CULTIVATION_FEED or ~/.omnia/...)")
    ap.add_argument("--offline", action="store_true", help="use cached lists only")
    ap.add_argument("--dry-run", action="store_true", help="print a summary, write nothing")
    args = ap.parse_args()

    repo = Path(args.repo).expanduser().resolve()
    cache = repo / "build" / "cultivation-cache"
    raw_problems, ratings_age = cached_json(cache / "zerotrac.json", ZEROTRAC_URL, args.offline)
    raw_all, _ = cached_json(cache / "leetcode_all.json", LC_ALL_URL, args.offline)
    problems = load_problems(raw_problems)
    difficulty, by_title = load_difficulty(raw_all)
    for p in problems:  # zerotrac titles also resolve contest files when LeetCode's list is absent
        by_title.setdefault(norm_title(p["title"] or ""), p["id"])

    attempts, skipped = collect_attempts(repo, contest_lookup(problems), by_title)
    for rel in skipped:
        warn(f"skipped {rel}: could not resolve its LeetCode id")

    feed = {
        "schema": SCHEMA,
        "generated_at": datetime.now().astimezone().isoformat(timespec="seconds"),
        "repo": str(repo),
        "commit": git(repo, "rev-parse", "HEAD") or None,
        "attempts": attempts,
        "problems": problems,
        "difficulty": difficulty,
    }
    out = output_path(args.out)
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


if __name__ == "__main__":
    main()
