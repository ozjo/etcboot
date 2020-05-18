#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>

#include "etcboot.h"

#define NOT_CMD    UINT8_MAX

typedef struct options {
	uint8_t chip;
	uint8_t bootmode;
	int fd;
	char dev[FILENAME_MAX];
} opt_t;

const char *_prog = "etcboot";
const char *commands[] = {
	"address",
	"image",
	"jump",
	"download",
	"run"
};

static const char *pcipathlist[] = {
	"/sys/bus/pci/devices/0000:%s/resource0",
	"/sys/bus/pci/devices/%s/resource0",
};

static const char *usbpathlist[] = {
	"/dev/%s"
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

static int search_file_descriptor(char *dev, uint8_t bootmode)
{
	int fd = -1, i = 0;
	char path[FILENAME_MAX];
	const char **pathlist;

	if (bootmode == PCIEBOOT)
		pathlist = pcipathlist;
	else if (bootmode == UARTBOOT)
		pathlist = usbpathlist;
	else
		goto out;

	while (pathlist[i]) {
		sprintf(path, pathlist[i], dev);
		if ((fd = open(path, O_RDWR)) != -1)
			goto out;

		i++;
	}

out:
	return fd;
}

static int load_fw(char *path, uint8_t *fw, uint32_t len)
{
	int retval = EXIT_SUCCESS;

	return retval;
}

int parse_options(int c, char *v[], opt_t *o, bootcfg_t *b)
{
	struct option optlist_long[] = {
		{ "annapurna", no_argument,       NULL, 'A' },
		{ "everest",   no_argument,       NULL, 'E' },
		{ "pcie",      required_argument, NULL, 'p' },
		{ "uart",      required_argument, NULL, 'u' },
		{ "adderss",   required_argument, NULL, 'a' },
		{ "file",      required_argument, NULL, 'f' },
		{ "verbose",   required_argument, NULL, 'v' },
		{ "help",      no_argument,       NULL, 'h' },
		{ NULL, 0, 0, 0}
	};
	const char optlist[] = "p:u:a:f:v:";
	int retval = EXIT_SUCCESS, opt;

	memset(o, 0, sizeof(opt_t));
	memset(b, 0, sizeof(bootcfg_t));

#if (DEFAULT_CHIP == EVEREST)
	o->chip = EVEREST;
#else
	o->chip = ANNAPURNA;
#endif

	while ((opt = getopt_long(c, v, optlist, optlist_long, NULL)) != -1) {
		switch (opt) {
			case 'A':
				o->chip = ANNAPURNA;
				break;

			case 'E':
				o->chip = EVEREST;
				break;

			case 'p':
				o->bootmode = PCIEBOOT;
				strcpy(o->dev, optarg);
				break;

			case 'u':
				o->bootmode = UARTBOOT;
				strcpy(o->dev, optarg);
				break;

			case 'a':
				b->addr = strtoull(optarg, NULL, 16);
				break;

			case 'f':
				retval = load_fw(optarg, b->fw, b->len);
				if (retval != EXIT_SUCCESS)
					goto out;
				break;

			case '?':
			case 'h':
			default:
			    show_usage();
		}
	};

	if (o->bootmode == NOTSUPBOOT) {
		printf("Device was not entered.\n");
		show_usage();
		retval = EXIT_FAILURE;
	} else {
		o->fd = search_file_descriptor(o->dev, o->bootmode);
		if (o->fd == -1) {
			printf("%s is invalid device.\n", o->dev);
			retval = ENOENT;
			goto out;
		}
	}

out:
	return retval;
}

#if 0
int do_command(uint8_t cmd, opt_t o)
{
	if (o.bootmode == PCIEBOOT)
		return do_pciecmd();
	else if (o.bootmode == UARTBOOT)
		return do_uartcmd();
}
#endif

int main(int argc, char *argv[])
{
	int retval = EXIT_FAILURE;
	uint8_t cmd;
	opt_t opt;
	bootcfg_t cfg;

	if (argc < 2) {
		show_usage();
		return EXIT_SUCCESS;
	}

	cmd = parse_command(argv[1]);
	if (cmd == NOT_CMD) {
		show_usage();
		return EXIT_SUCCESS;
	}

	retval = parse_options(argc, argv, &opt, &cfg);
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
