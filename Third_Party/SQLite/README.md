# SQLite

This folder contains the SQLite amalgamation used by the default Appeal Storage
plugin.

- Source: https://www.sqlite.org/download.html
- Version: 3.53.1 / `sqlite-amalgamation-3530100`
- License: public domain

The build uses `sqlite3.c` and `sqlite3.h` directly so the default Storage
plugin does not need a separate SQLite runtime DLL.
