# Fake Build System - Fake BS
The Fake Build System, Fake BS, is a build system for building small c
projects.

# Building
Fake can be built both using an existing build of fake or using the
included Makefile.
```bash
# Using fake
fake

# Using make
make
```

If you want to install fake to your system, execute:
```bash
make install
```
**NOTE:** This will install fake into ~/.local/bin

# Usage
The Fake program builds a project in the current directory according to a
Fakefile. \
To get more information, run:
```
fake --help
```
To know how Fakefile works, check out examples in this codebase.

# LSP support
To get lsps to care about your fake configuration, use the 
[bear](https://github.com/rizsotto/Bear) project like below for a 
fakefile configured working directory:
```
bear -- fake
```
That will make a compile_commands.json file that most lsps automatically
detect and use.
