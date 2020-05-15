#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "etcboot.h"

#define NOT_CMD    UINT8_MAX

typedef struct option {
	uint8_t target;
	char dev[FILENAME_MAX];
	uint64_t addr;
	uint32_t len;
} opt_t;

const char *_prog = "etcboot";
const char *commands[] = {
	"address",
	"image",
	"jump",
	"download",
	"run"
};

void show_usage(void);
void show_usage_command(uint8_t cmd);

uint8_t parse_command(char *s)
{
	uint8_t c = 0;

	while (commands[c]) {
		if (!strcmp(commands[c], s))
			return c;

		c++;
	};

	return NOT_CMD;
}

int parse_options(char **s, opt_t *o)
{
	int retval = EXIT_FAILURE;

	memset(o, 0, sizeof(opt_t));
	/* getarg */

	return retval;
}

#if 0
int do_command(uint8_t cmd, opt_t o)
{
	if (o->target == PCIEBOOT)
		return do_pciecmd();
	else if (o->target == UARTBOOT)
		return do_uartcmd();
}
#endif

int main(int argc, char *argv[])
{
	int retval = EXIT_FAILURE;
	uint8_t cmd;
	opt_t opt;

	if (argc < 2) {
		show_usage();
		return EXIT_SUCCESS;
	}

	cmd = parse_command(argv[1]);
	if (cmd == NOT_CMD) {
		show_usage();
		return EXIT_SUCCESS;
	} else if (argc == 2) {
		if (cmd != CMD_JUMP) {
			show_usage_command(cmd);
			return EXIT_SUCCESS;
		}
	}

	retval = parse_options(&argv[2], &opt);
	if (retval) {
		printf("Parse options failed\n");
		return retval;
	}

	/* retval = do_command(cmd, opt); */

	return retval;
}

void show_usage(void)
{
	printf("%s [command] [-p pci_id | -u usb_dev] [options]\n", _prog);
}

void show_usage_command(uint8_t c)
{
	printf("command is %x\n", c);
}
