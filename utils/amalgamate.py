#!/usr/bin/env python3
"""
amalgamate.py — Produce a single-file amalgamation of the ysc::matrix library.

Usage:
    python3 utils/amalgamate.py [-o OUTPUT]

Options:
    -o OUTPUT   Write the amalgamated header to OUTPUT (default: stdout)
"""

import argparse
import re
import sys
from datetime import date
from pathlib import Path

# Headers to amalgamate, in dependency order
HEADERS = [
    "matrix_detail.hpp",
    "matrix_view.hpp",
    "matrix.hpp",
]

# Include patterns that reference internal library headers (to be stripped)
INTERNAL_INCLUDES = re.compile(
    r'^\s*#\s*include\s*[<"](?:matrix_detail\.hpp|matrix_view\.hpp|matrix\.hpp)[">\s]*$'
)


def amalgamate(src_dir: Path) -> str:
    """Return the full text of the amalgamated header."""
    today = date.today().isoformat()
    lines = [
        "// =============================================================================",
        "// matrix-amalgamated.hpp",
        "// Auto-generated amalgamation of the ysc::matrix library.",
        f"// Generated on: {today}",
        "// DO NOT EDIT — regenerate with:  python3 utils/amalgamate.py -o matrix-amalgamated.hpp",
        "// =============================================================================",
        "",
    ]

    for header_name in HEADERS:
        header_path = src_dir / header_name
        if not header_path.exists():
            raise FileNotFoundError(f"Header not found: {header_path}")

        lines.append(f"// === BEGIN {header_name} ===")
        raw = header_path.read_text(encoding="utf-8")
        for line in raw.splitlines():
            if INTERNAL_INCLUDES.match(line):
                # Drop internal #include directives — the content is already inlined above.
                continue
            lines.append(line)
        lines.append(f"// === END {header_name} ===")
        lines.append("")

    return "\n".join(lines) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Produce a single-file amalgamation of the ysc::matrix library."
    )
    parser.add_argument(
        "-o",
        metavar="OUTPUT",
        default=None,
        help="Output file path (default: stdout)",
    )
    args = parser.parse_args()

    # Locate src/include/ relative to this script's parent directory
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    src_dir = repo_root / "src" / "include"

    content = amalgamate(src_dir)

    if args.o is None:
        sys.stdout.write(content)
    else:
        out = Path(args.o)
        out.write_text(content, encoding="utf-8")
        print(f"Written: {out}", file=sys.stderr)


if __name__ == "__main__":
    main()
