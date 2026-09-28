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
// @silent()
// @combine(var, sep)

CC = cc,
SRCS = @find(src, "*.c"),
OBJS = @pathsub($SRCS, "src/*.c", "build/*.o"),

label build: fake {}

rule fake: $OBJS {
	$CC $OBJS -o fake
}

multi $OBJS: $SRCS {
	@mkdir(build),
	$CC $deps -c -o $name
}

label clean {
	rm -f fake,
	rm -rf build
}

label run {
	./fake
}
