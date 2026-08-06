CC := cc
NAME := fake

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,build/%.o,$(SRCS))

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) -Wall -c $< -o $@

.PHONY: run clean install uninstall

run: $(NAME)
	@./fake

install: $(NAME)
	mkdir -p ~/.local/bin
	cp $(NAME) ~/.local/bin

uninstall:
	rm -f ~/.local/bin/$(NAME)

clean:
	rm -f fake
	rm -rf build

