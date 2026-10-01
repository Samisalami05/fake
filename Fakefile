// TODO: deps should consist of ids
// TODO: look for circular dependencies
// TODO: prepass should look for invalid builtins and macros
// TODO: maybe recursive Fakefiles???
// TODO: maybe preprocessor on env variables (#if)

// Ideas
//  * Ability to add progress bars and text on labels when they are executed
//		(better) add flag for progress bar

// built-in functions
// @echo()
// @mkdir()
// @mkfile()
// @rm()
// @concat()
// @pathsub()
// @substr()
// @env()
// @silent()            # Executes given commands without printing
// @combine(var, sep)
// @option(msg, opts)   # Displays an option list
// @selection()

CC = cc,
NAME = fake,
SRCS = @find(src, "*.c"),
OBJS = @pathsub($SRCS, "src/*.c", "build/*.o"),

LDFLAGS = -lncursesw -rdynamic,
CFLAGS = -Wall -ggdb -finstrument-functions,

label build: $NAME {}

rule $NAME: $SRCS $OBJS {
	$CC $OBJS -o $NAME $LDFLAGS,
}

multi $OBJS: $SRCS {
	@mkdir(@dirname($name)),
	$CC $deps $CFLAGS -c -o $name
}

label clean {
	rm -f fake,
	rm -rf build
}

label run {
	./fake
}

label install {
	sudo cp fake @if(@stored("INSTALL_DIR"),
		@read("INSTALL_DIR"),
		@store("INSTALL_DIR", @option(
			"Where do you want to install it?",
			"/usr/bin" "/usr/local/bin"
		))
		@read("INSTALL_DIR")
	)
}
