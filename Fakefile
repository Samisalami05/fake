// TODO: make dependencies work
// TODO: add identifier path parsing, for example path/to/*.c or path/to/**.h
// TODO: variables
// TODO: env variables
// TODO: inbuilt functions, for example replace(var, pattern, new)
// TODO: maybe recursive Fakefiles???
// TODO: maybe preprocessor on env variables (#if)

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
// @mkdir()
// @file()
// @rm()
// @pathsub(main.c camera.c, ".o"
// @substr()

TEST = wow,

test {
	gcc -Wall test.c -o test
}

run: test {
	./test
}
