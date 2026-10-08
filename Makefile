CC := cc
NAME := fake

SRCS := $(wildcard src/*.c) $(wildcard src/builtins/*.c)
OBJS := $(patsubst src/%.c,build/%.o,$(SRCS))

LDFLAGS := -lncursesw
CFLAGS := -Wall -ggdb

# Detect Windows
ifeq ($(OS),Windows_NT)
    EXE := .exe

    MKDIR_BUILD = powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path 'build','build/builtins' | Out-Null"
    RM_FILE = powershell -NoProfile -Command "if (Test-Path '$(NAME)$(EXE)') { Remove-Item -Force -ErrorAction SilentlyContinue '$(NAME)$(EXE)' }"
    RM_DIR = powershell -NoProfile -Command "if (Test-Path 'build') { Remove-Item -Recurse -Force -ErrorAction SilentlyContinue 'build' }"

	INSTALL = powershell -NoProfile -Command "Copy $(NAME)$(EXE) C:\msys64\usr\local\bin"
	UNINSTALL = powershell -NoProfile -Command "Remove-Item -Force -ErrorAction SilentlyContinue C:\msys64\usr\local\bin\$(NAME)$(EXE)"
else
    EXE :=
    
    MKDIR_BUILD = mkdir -p build build/builtins
    RM_FILE = rm -f $(NAME)
    RM_DIR = rm -rf build

	INSTALL = sudo cp $(NAME) /usr/local/bin
	UNINSTALL = rm -f /usr/local/bin/$(NAME)
endif

$(NAME)$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

build/%.o: src/%.c
	@$(MKDIR_BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean install uninstall

install: $(NAME)$(EXE)
	$(INSTALL)

uninstall:
	$(UNINSTALL)

clean:
	$(RM_FILE)
	$(RM_DIR)
