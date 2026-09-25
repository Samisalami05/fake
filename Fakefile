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

CC = cc,
SRCS = @find(src, "*.c"),
OBJS = @pathsub($SRCS, "src/*.c", "tmp/*.o"),

label build: test {}

rule test: $OBJS {
	$CC $OBJS -o test
}

multi $OBJS: $SRCS {
	@mkdir(tmp),
	$CC #deps -c -o #name
}

label clean {
	rm -f test,
	rm -f test.o,
	rm -rf tmp
}
