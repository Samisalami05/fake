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
// @list()

CC = cc,
NAME = fake,
SRCS = @find(src, "*.c"),
OBJS = @pathsub($SRCS, "src/*.c", "build/*.o"),

CHOICE = @option("/usr/bin" "/usr/local/bin" "other"),
w_ = @echo($CHOICE),
PATH = @if($CHOICE "other", @input(), $CHOICE),

w_ = @if(@stored("cc") false,
	@store("cc", @prompt("use gcc?", "gcc", "clangd"))
),
CC = @read("cc"),

w_ = @echo($CC),


label build: $NAME {}

rule $NAME: $OBJS {
	$CC $OBJS -o $NAME,
}

multi $OBJS: $SRCS {
	@mkdir(@dirname($name)),
	$CC $deps -c -o $name
}

label clean {
	rm -f fake,
	rm -rf build
}

label run {
	./fake
}
