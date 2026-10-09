#!/usr/bin/env -S uv run --script
# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Jing Haihan
# /// script
# requires-python = ">=3.9"
# dependencies = ["semver==3.0.4"]
# ///

import argparse
import subprocess
from pathlib import Path

from semver import Version


ROOT = Path(__file__).resolve().parent.parent


def git(*args: str, capture: bool = False) -> str:
    result = subprocess.run(
        ["git", *args], cwd=ROOT, check=True, text=True, capture_output=capture
    )
    return result.stdout.strip() if capture else ""


def main() -> None:
    parser = argparse.ArgumentParser(description="Release FETH Overlay.")
    parser.add_argument("bump", choices=("patch", "minor", "major"))
    args = parser.parse_args()

    if git("branch", "--show-current", capture=True) != "main":
        raise SystemExit("Switch to main before releasing.")
    if git("status", "--porcelain", capture=True):
        raise SystemExit("Commit your changes before releasing.")

    git("fetch", "origin", "main", "--tags")
    if git("rev-parse", "HEAD", capture=True) != git(
        "rev-parse", "origin/main", capture=True
    ):
        raise SystemExit("Synchronize main with origin/main before releasing.")

    version_path = ROOT / "VERSION"
    current = Version.parse(version_path.read_text().strip())
    version = getattr(current, f"bump_{args.bump}")()
    tag = f"v{version}"
    if git("tag", "--list", tag, capture=True):
        raise SystemExit(f"Tag already exists: {tag}")

    print(f"Releasing v{current} -> {tag}", flush=True)
    version_path.write_text(f"{version}\n")
    git("add", "VERSION")
    git("commit", "-m", f"chore: release {tag}")
    git("tag", "-a", tag, "-m", tag)
    git("push", "--atomic", "origin", "main", tag)
    print(f"Pushed {tag}; GitHub Actions will build and publish the release.")


if __name__ == "__main__":
    main()
