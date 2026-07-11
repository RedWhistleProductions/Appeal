# Appeal

Appeal is a general use programming language, with a limited set of commands, designed to be expanded through the use of Modules, Plugins, and Extensions. Please see the documentation for details. (Coming soon on the Red Whistle Productions GitHub.)

## Current Scope

- Core C++ runtime
- Core Appeal script loading
- Dynamic Extension loading with `Add_Extension`
- Minimal `Resources/Appeal/Main.Appeal` and `Resources/Appeal/Setup/Config.Appeal`
- No bundled modules or plugins

## Extensions

Extensions are dynamic libraries that add new Appeal keywords without requiring a separate plugin. They use the same command syntax style as Modules, but the implementation is built into the Extension itself.

Appeal loads an Extension with:

```appeal
Add_Extension Name
```

The runtime looks for the Extension at:

```text
Resources/Extensions/Name/Name.dll
Resources/Extensions/Name/Name.so
```

An Extension should export `Interpreter(Data_Source *Data)`. It may also export `Init`, `Init_Globals`, `Set_Function_Runner`, and `Shutdown`.

## Building From Source

On Windows with `w64devkit`:

```sh
make -f Makefile
```

On Linux:

```sh
make -f Makefile
```

Run from the project folder:

```sh
./Appeal
```

Installed Linux packages launch through the `appeal` wrapper, which sets `APPEAL_RESOURCE_FOLDER` so the runtime can find packaged resources.

## Release Packaging

Generated release files should not be committed. They belong in GitHub Releases.

Planned release assets:

- Source archive
- Windows portable zip
- Windows installer exe
- Linux tar.gz
- Linux deb package
- Linux Flatpak bundle
- Linux rpm package later
- macOS zip/dmg later through GitHub Actions

The current package line is `1.0.0-beta.1`, intended for core runtime and packaging testing before the final `1.0.0` release.

## License

Appeal is licensed under the MIT License. See `LICENSE` for details.
