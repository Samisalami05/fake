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

else

    EXE :=
    
    MKDIR_BUILD = mkdir -p build build/builtins
    RM_FILE = rm -f $(NAME)
    RM_DIR = rm -rf build

endif

$(NAME)$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

build/%.o: src/%.c
	@$(MKDIR_BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run clean install uninstall

run: $(NAME)$(EXE)
ifeq ($(OS),Windows_NT)
	.\$(NAME)$(EXE)
else
	./$(NAME)
endif

install: $(NAME)$(EXE)
ifeq ($(OS),Windows_NT)
	@echo "install is not supported natively on Windows"
	@echo "Copy $(NAME)$(EXE) somewhere in your PATH manually."
else
	sudo cp $(NAME) /usr/local/bin
endif

uninstall:
ifeq ($(OS),Windows_NT)
	@echo "uninstall is not supported natively on Windows"
else
	rm -f /usr/local/bin/$(NAME)
endif

clean:
	$(RM_FILE)
	$(RM_DIR)
