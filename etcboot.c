#include <stdio.h>

const char *_prog = "etcboot";

void show_usage(void);

int main(int argc, char *argv[])
{
	int retval;

	if (argc < 2)
		show_usage();

	return retval;
}

void show_usage(void)
{
	printf("Welcome %s\n", _prog);
}

