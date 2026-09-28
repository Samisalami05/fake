#include "fake.h"
#include "parse_args.h"

int main(int argc, char **argv) {
	FakeConfig conf = {0};
	parse_args(argv, &conf);

	Fakefile ff = {0};
	if (!fake_open(conf, &ff))
		return 1;
	
	if (!fake_exec(&ff))
		return 1;

	fake_close(&ff);
	return 0;
}
