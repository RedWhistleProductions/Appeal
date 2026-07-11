#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
VERSION="$(tr -d '\r\n' < "$ROOT/VERSION")"
DEB_VERSION="${VERSION/-beta./~beta}"
ARCH="${ARCH:-amd64}"
PACKAGE="appeal"
STAGE="${TMPDIR:-/tmp}/${PACKAGE}_${DEB_VERSION}_${ARCH}"
DEBIAN="$STAGE/DEBIAN"
OUTPUT="$ROOT/Dist/${PACKAGE}_${DEB_VERSION}_${ARCH}.deb"

cd "$ROOT"
make -f Makefile clean
make -f Makefile

rm -rf "$STAGE"
mkdir -p "$DEBIAN"

make -f Makefile install DESTDIR="$STAGE" PREFIX=/usr
mkdir -p "$STAGE/usr/share/doc/$PACKAGE"
cp LICENSE "$STAGE/usr/share/doc/$PACKAGE/copyright"

cat > "$DEBIAN/control" <<CONTROL
Package: $PACKAGE
Version: $DEB_VERSION
Section: games
Priority: optional
Architecture: $ARCH
Maintainer: Red Whistle
Description: Appeal script runtime
 Appeal is a small script runtime for building Appeal applications.
CONTROL

find "$STAGE" -type d -exec chmod 755 {} +
find "$STAGE" -type f -exec chmod 644 {} +
chmod 755 "$STAGE/usr/bin/appeal"
chmod 755 "$STAGE/usr/lib/appeal/Appeal"

mkdir -p "$ROOT/Dist"
dpkg-deb --build "$STAGE" "$OUTPUT"
