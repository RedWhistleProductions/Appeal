#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="$(tr -d '\r\n' < "$ROOT/VERSION")"
STAGE="${TMPDIR:-/tmp}/Appeal-${VERSION}-linux-x64"
OUTPUT="$ROOT/Dist/Appeal-${VERSION}-linux-x64.tar.gz"

cd "$ROOT"
make -f Makefile clean
make -f Makefile

rm -rf "$STAGE"
mkdir -p "$STAGE"

cp Appeal "$STAGE/"
cp -R Resources "$STAGE/"
cp README.md "$STAGE/"
cp LICENSE "$STAGE/"
cp VERSION "$STAGE/"

mkdir -p "$ROOT/Dist"
tar -czf "$OUTPUT" -C "$(dirname "$STAGE")" "$(basename "$STAGE")"
echo "Created $OUTPUT"
