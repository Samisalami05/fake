CC := cc
NAME := fake

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,build/%.o,$(SRCS))

$(NAME): $(OBJS)
	$(CC) $(OBJS) -rdynamic -o $(NAME)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) -Wall -ggdb -finstrument-functions -c $< -o $@

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

