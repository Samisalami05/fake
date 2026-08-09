// TODO: make dependencies work
// TODO: add identifier path parsing, for example path/to/*.c or path/to/**.h
// TODO: variables
// TODO: env variables
// TODO: inbuilt functions, for example replace(var, pattern, new)
// TODO: maybe recursive Fakefiles???
// TODO: maybe preprocessor on env variables (#if)

test {
	gcc -Wall test.c -o test
}

run: test {
	./test
}
