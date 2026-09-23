// TODO: make dependencies work
// TODO: add identifier path parsing, for example path/to/*.c or path/to/**.h
// TODO: variables
// TODO: env variables
// TODO: inbuilt functions, for example replace(var, pattern, new)
// TODO: maybe recursive Fakefiles???
// TODO: maybe preprocessor on env variables (#if)

// Ideas
//  * Ability to add progress bars and text on labels when they are executed
//		(better) add flag for progress bar


// shell commands
// TEST = { find "src" -name "*.c" }
// TEST = @shell(find "src" -name "*.c")

// file glob
// TEST = src/**.c
// TEST = @find(src, "c")

// TEST = "foo" "bar"
// TEST_NEW = $TEST "baz" -> "foo" "bar" "baz"
// TEST_NEW = $TEST + "baz" -> "foobaz" "barbaz"

// built-in functions
// @echo()
// @mkdir()
// @mkfile()
// @rm()
// @concat()
// @pathsub(main.c camera.c, ".o"
// @substr()
// @env()

CC = gcc @echo(),
CXX = $CC,

rule main wowe: main.c {
	@echo(#names),
	@echo(#deps),
	// $CC main.c -o main,
}

multi test ayo: test.c ayo.c {
	@echo("wwo", "wowe"),
}

// CC = gcc,
// NAME = main,

// SRCS = @find(src *.c),
// OBJS = @pathsub($SRCS, src/*.c, build/*.o),

// rule $NAME: main.c {
//	@mkdir("build"),
//	$CC main.c -o main
//}

//multi $OBJS: $SRCS {
//	@mkdir(#name),
//	$CC #deps -o #name
//}

//label run: $NAME {
//	./$NAME
//}

