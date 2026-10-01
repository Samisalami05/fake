CC := cc
NAME := fake

SRCS := $(wildcard src/*.c) $(wildcard src/builtins/*.c)
OBJS := $(patsubst src/%.c,build/%.o,$(SRCS))

LDFLAGS := -lncursesw -rdynamic
CFLAGS := -Wall -ggdb -finstrument-functions

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

build/%.o: src/%.c
	@mkdir -p build build/builtins
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run clean install uninstall

run: $(NAME)
	@./fake

install: $(NAME)
	sudo cp $(NAME) /usr/local/bin

uninstall:
	rm -f /usr/local/bin/$(NAME)

clean:
	rm -f fake
	rm -rf build

