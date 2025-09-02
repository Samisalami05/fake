# Fake Build System - Fake BS
The Fake Build System, Fake BS, is a build system for building small c projects.

# Building
To build fake without access to fake, you can use the build_manual.sh file:
```bash
./build_manual.sh
```
  
If you want to install fake to a system path, you execute:
```bash
./install.sh
```

# Usage
The Fake program builds a project in the current directory according to a fakefile.
To get more information, run:
```
fake --help
```

# LSP support
To get lsps to care about your fake configuration, use the [bear](https://github.com/rizsotto/Bear) project like below for a fakefile configured working directory:
```
bear -- fake
```
That will make a compile_commands.json file that most lsps automatically detect and use.
