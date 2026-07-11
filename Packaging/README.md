# Appeal Packaging

Packaging files live here; generated artifacts go into `Dist/` and should be attached to GitHub Releases instead of committed.

The current beta version is `1.0.0-beta.1`. Debian packages use `1.0.0~beta1` so the beta sorts before the final `1.0.0` package.

## Windows

- `Windows/Build-Portable.ps1` builds `Dist/Appeal-<version>-windows-x64.zip`.
- `Windows/Appeal.iss` is an Inno Setup recipe that builds a normal `.exe` installer.

## Linux

- `Linux/build-deb.sh` builds a Debian package for Mint and other Debian-based systems.
- `Linux/org.redwhistle.appeal.yml` is the Flatpak manifest.
- `Linux/org.redwhistle.appeal.desktop` and `Linux/org.redwhistle.appeal.metainfo.xml` are desktop packaging metadata.

The Linux package installs the executable to `/usr/lib/appeal/Appeal` and uses `/usr/bin/appeal` as a wrapper that points the runtime at `/usr/share/appeal/Resources`.
