# Appeal

Appeal is a general use programming language, with a limited set of commands, designed to be expanded through the use of Modules, Plugins, and Extensions. Please see the documentation for details.

## About Modules and Plugins
Appeal Libraries come in 2 parts a Module and a Plugin. Each Module extends the Appeal language by adding a new interpreter with it's own set of keywords and an API for plugins. Plugins are designed to work with a specific Module and provide the implementation for it's keywords. For example the Graphics Module can use a Direct X plugin, an OpenGL plugin, or a Vulcan plugin, to create a 3D environment, but the Apeal script just uses the same keywords no matter which plugin is used. This way Appeal stays platform independent through Modules, while plugins allow for platform specific optimizations.

## About Extensions
An Extension works like a Module with a built in Plugin. Extensions allow a specific C or C++ library to be ported to Appeal, or for a C or C++ app to be interfaced with Appeal.

## Installing Modules
Create a folder with the name of the Module inside the Modules folder found inside the Resources folder. Copy the file(s) here. Optionally you can git clone the source in this folder and type make.

## Installing Plugins
Install the coresponding Module first, then create a folder for the plugin inside the folder for the coresponding Module. Copy the file(s) here. Optionally you can git clone the source in this folder and type make.

## Installing Extensions
Create a folder with the name of the Extension inside the Extensions folder inside of the Resources Folder.

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

Run a script from any folder by passing the script path:

```sh
./Appeal path/to/Script.Appeal
appeal /home/user/Documents/AppealApps/Test.Appeal
```

If no script path is provided, Appeal runs `Main.Appeal` from its resource folder. Source builds use the local `Resources` folder when run from the project folder. Installed Windows builds use `Documents\Appeal\Resources`. Installed Linux packages launch through the `appeal` wrapper, which creates `~/Documents/Appeal/Resources` from the packaged resource templates on first run.

## Documentation

Open [Appeal HTML documentation](Docs/index.html) for the language reference, module API standard, and module functions and globals.
