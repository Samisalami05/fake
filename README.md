# Fake Build System - Fake BS
The Fake Build System, Fake BS, is a build system for building source code. Fake mainly targets c/cpp but can be used for any tasks surrounding the shell.

The build system is inspired by the [GNU Make](https://www.gnu.org/software/make/) and contains a similar syntax and functionality.

# Why use fake instead of make

In my opinion, Makefile is a great build system with a lot of cleaver features but falls short when you want to support building from different shells and operating systems. For example if you need to create a directory, the name of the program depends on the shell. On Windows Powershell, the program is `New-Item -ItemType Directory` and in bash its `mkdir`. This lead to annoying workarounds like creating functions in make which depending on the operating system, used the correct program.

To solve this problem, fake contains built in functions that work on all operating systems. This leads to less code and faster execution time because executing a program like `mkdir`, creates a new process as well which takes a lot of time.

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
**NOTE:** This will install fake into `/usr/local/bin`

# Usage
The Fake program builds a project in the current directory according to a
Fakefile. \
To get more information, run:
```
fake --help
```
To know how Fakefile works, check out examples in this codebase.

# Syntax Highlighting
* [tree-sitter](https://github.com/Samisalami05/tree-sitter-fake)

# LSP support
To get lsps to care about your fake configuration, use the 
[bear](https://github.com/rizsotto/Bear) project like below for a 
fakefile configured working directory:
```
bear -- fake build
```
That will make a compile_commands.json file that most lsps automatically
detect and use.
