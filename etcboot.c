#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>

#include "etcboot.h"
#include "connectdev.h"
#include "pciecmd.h"
#include "uartcmd.h"

/* etc boot command arguments */
#define ARGS_BIFOFF_SUBOPC     (0)
#define ARGS_BITOFF_ADDR       (1)
#define ARGS_BITOFF_FILE       (2)
#define ARGS_BITOFF_RUN        (3)
#define ARGS_BITOFF_TCGRP      (4)
#define ARGS_BITOFF_TCNUM      (5)

#define ARGS_DISABLE           (0)

#define CMD_ARGS_MASK_SUBOPC   (1 << ARGS_BIFOFF_SUBOPC)
#define CMD_ARGS_MASK_ADDR     (1 << ARGS_BITOFF_ADDR)
#define CMD_ARGS_MASK_FILE     (1 << ARGS_BITOFF_FILE)
#define CMD_ARGS_MASK_RUN      (ARGS_DISABLE)
#define CMD_ARGS_MASK_TCGRP    (1 << ARGS_BITOFF_TCGRP)
#define CMD_ARGS_MASK_TCNUM    (1 << ARGS_BITOFF_TCNUM)

#define is_arg_set(var, mask)  ((var & mask) & mask)

const char *_prog = "etcboot";

#if (CHIP == EVEREST)
uint8_t chip = EVEREST;
#else
uint8_t chip = ANNAPURNA;
#endif

typedef struct options {
	char dev[FILENAME_MAX];
	char fwpath[FILENAME_MAX];
	int verbose;
	char logpath[FILENAME_MAX];
	FILE *flog;
} opt_t;

typedef struct commands {
	uint8_t code;
	char *str;
} cmd_t;

cmd_t etcboot_cmd[] = {
	{
		CMD_STATUS,
		"status",
	},
	{
		CMD_ADDRESS,
		"address",
	},
	{
		CMD_DOWNLOAD,
		"download",
	},
	{
		CMD_RUN,
		"run",
	},
	{
		CMD_PERI_BIST,
		"peri-bist",
	},
	{
		CMD_DRAM_BIST,
		"dram-bist",
	},
	{
		CMD_NAND_BIST,
		"nand-bist",
	},
	{
		NOTCMD,
		NULL,
	}
};

enum maincmds_idx {
	MAINCMD_ADDR = 0,
	MAINCMD_DOWNLOAD,
	MAINCMD_RUN
};

void show_usage(void);
void show_usage_command(uint8_t cmd);

void init_bootcfg(bootcfg_t *b)
{
	memset(b, 0, sizeof(bootcfg_t));
	b->fw = NULL;

	b->aes_txn_sz = 4096;
	b->subopc = INVAL_SUBOPC;

	b->addr = INVAL_ADDR;
#if DEFAULT_BOOTMODE == PCIEBOOT
	b->target = PCIEBOOT;
#elif DEFAULT_BOOTMODE == UARTBOOT
	b->target = UARTBOOT;
#else
	b-target = NOTSUPBOOT;
#endif
}

cmd_t parse_command(char *s)
{
	uint8_t c = 0;

	while (etcboot_cmd[c].str) {
		if (!strcmp(etcboot_cmd[c].str, s)) {
			break;
		}

		c++;
	};

	return etcboot_cmd[c];
}

static uint32_t load_fw(char *path, uint8_t **fw)
{
	uint32_t readb, flen, blen, alp;
	FILE *f_fw = NULL;

	if ((f_fw = fopen(path, "r")) == NULL) {
		return 0;
	}

	fseek(f_fw, 0, SEEK_END);
	flen = (uint32_t) ftell(f_fw);
	fseek(f_fw, 0, SEEK_SET);

	/* pad aligned 4 byte */
	alp = flen % 4;
	blen = alp ? flen + (4 - alp) : flen;

	*fw = (uint8_t *) malloc(sizeof(uint8_t) * blen);
	memset(*fw, 0, sizeof(uint8_t) * blen);

	readb = (uint32_t) fread(*fw, 1, flen, f_fw);
	if (readb != flen) {
		printf("file read failed (readb: %d, flen: %d)\n", readb, flen);
		return 0;
	}

	fclose(f_fw);

	return blen;
}

static int validate_options(opt_t o, bootcfg_t *b, cmd_t c)
{
	int retval = EXIT_SUCCESS;
	uint32_t margs = ARGS_DISABLE; /* mandatory arguments */

	if (b->target == NOTSUPBOOT) {
		printf("not set bootmode\n");
		retval = EINVAL;
		goto err;
	}

	switch (c.code) {
		case CMD_STATUS:
			margs = CMD_ARGS_MASK_SUBOPC;
			break;

		case CMD_ADDRESS:
			margs = CMD_ARGS_MASK_ADDR;
			break;

		case CMD_DOWNLOAD:
			if (b->addr != INVAL_ADDR) {
				margs |= CMD_ARGS_MASK_ADDR;
			}

			margs = CMD_ARGS_MASK_FILE;

			if (b->wrun) {
				margs |= CMD_ARGS_MASK_RUN;
			}

			break;

		case CMD_RUN:
			margs = CMD_ARGS_MASK_RUN;
			break;

		case CMD_PERI_BIST:
		case CMD_DRAM_BIST:
		case CMD_NAND_BIST:
			if (b->bistcmd == BISTCMD_RUNTC) {
				margs = CMD_ARGS_MASK_TCNUM | CMD_ARGS_MASK_TCGRP;
			}
			break;

		default:
			/* Do Nothing */
			margs = ARGS_DISABLE;
			break;
	}

	if (is_arg_set(margs, CMD_ARGS_MASK_ADDR)) {
		if (b->addr == INVAL_ADDR) {
			printf("address is empty\n");
			retval = EINVAL;
			goto err;
		}
	}

	if (is_arg_set(margs, CMD_ARGS_MASK_FILE)) {
		if (o.fwpath[0] == 0) {
			printf("file path is empty\n");
			/* print usage */
			retval = EINVAL;
			goto err;
		}

		b->len = load_fw(o.fwpath, &b->fw);
		if (b->len == 0) {
			printf("fw load failed (%s)\n", o.fwpath);
			retval = EINVAL;
			goto err;
		}
	}

	if (is_arg_set(margs, CMD_ARGS_MASK_SUBOPC)) {
		if (b->subopc == INVAL_SUBOPC) {
			printf("subopcode is empty\n");
			retval = EINVAL;
			goto err;
		}
	}

	if (is_arg_set(margs, CMD_ARGS_MASK_TCGRP)) {
		if (b->tcgrp == INVAL_TCGRP) {
			printf("testcase group is empty\n");
			retval = EINVAL;
			goto err;
		}
	}

	if (is_arg_set(margs, CMD_ARGS_MASK_TCNUM)) {
		if (b->tcnum == INVAL_TCNUM) {
			printf("tescase number is empty\n");
			retval = EINVAL;
			goto err;
		}
	}

	if (o.logpath[0] != 0) {
		FILE *f_log = NULL;
		if ((f_log = fopen(o.logpath, "rw")) == NULL) {
			printf("log file open failed (%s)\n", o.logpath);
			retval = EINVAL;
			goto err;
		}
	}

err:
	return retval;
}

static int parse_options(int argc, char *argv[], opt_t *o, bootcfg_t *b, cmd_t *c)
{
	int opt;

	const char optlist[] = "AEp:u:a:f:x:s:Rg:n:ITl:vh";
	struct option optlist_long[] = {
		{ "annapurna", no_argument,       NULL, 'A' },
		{ "everest",   no_argument,       NULL, 'E' },
		{ "pcie",      required_argument, NULL, 'p' },
		{ "uart",      required_argument, NULL, 'u' },
		{ "adderss",   required_argument, NULL, 'a' },
		{ "file",      required_argument, NULL, 'f' },
		{ "aestxnsz",  required_argument, NULL, 'x' },
		{ "subopc",    required_argument, NULL, 's' },
		{ "run",       no_argument,       NULL, 'R' },
		{ "tc-group",  required_argument, NULL, 'g' },
		{ "tc-num",    required_argument, NULL, 'n' },
		{ "get-info",  no_argument,       NULL, 'I' },
		{ "get-tc",    no_argument,       NULL, 'T' },
		{ "log",       required_argument, NULL, 'l' },
		{ "verbose",   no_argument,       NULL, 'v' },
		{ "help",      no_argument,       NULL, 'h' },
		{ NULL, 0, 0, 0}
	};

	memset(o, 0, sizeof(opt_t));

	while ((opt = getopt_long(argc, argv, optlist, optlist_long, NULL)) != -1) {
		switch (opt) {
			case 'A':
				chip = ANNAPURNA;
				break;

			case 'E':
				chip = EVEREST;
				break;

			case 'p':
				b->target = PCIEBOOT;
				strcpy(o->dev, optarg);
				break;

			case 'u':
				b->target = UARTBOOT;
				strcpy(o->dev, optarg);
				break;

			case 'a':
				b->addr = strtoull(optarg, NULL, 16);
				break;

			case 'f':
				strcpy(o->fwpath, optarg);
				break;

			case 'x':
				b->aes_txn_sz = atoi(optarg);
				break;

			case 's':
				b->subopc = atoi(optarg);
				break;

			case 'R':
				b->wrun = true;
				break;

			case 'g':
				b->tcgrp = atoi(optarg);
				break;

			case 'n':
				b->tcnum = atoi(optarg);
				break;

			case 'I':
				b->bistcmd = BISTCMD_GETBISTINFO;
				break;

			case 'T':
				b->bistcmd = BISTCMD_GETTC;
				break;

			case 'l':
				strcpy(o->logpath, optarg);
				break;

			case 'v':
				break;

			case '?':
			case 'h':
			default:
			    show_usage();
		}
	};

	return validate_options(*o, b, *c);
}

int do_command(opt_t o, uint8_t cmd, bootcfg_t *c)
{
	if (c->target == PCIEBOOT) {
		return do_pciecmd(cmd, c);
	} else if (c->target == UARTBOOT) {
		return do_uartcmd(cmd, c);
	} else {
		return ENODEV;
	}
}

void cleanup(bootcfg_t b, opt_t o)
{
	if (b.fw) {
		free(b.fw);
	}

	if (o.flog) {
		fclose(o.flog);
	}
}

int main(int argc, char *argv[])
{
	int retval = EXIT_FAILURE;
	cmd_t cmd;
	opt_t opt;
	bootcfg_t cfg;

	if (argc < 2) {
		show_usage();

		return EXIT_SUCCESS;
	}

	init_bootcfg(&cfg);

	cmd = parse_command(argv[1]);
	if (cmd.code == NOTCMD) {
		show_usage();

		return EXIT_SUCCESS;
	}

	retval = parse_options(argc, argv, &opt, &cfg, &cmd);
	if (retval) {
		printf("Parse options failed\n");

		return retval;
	}

	retval = connect_device(opt.dev, &cfg);
	if (retval) {
		printf("Connect device failed\n");

		return retval;
	};

	retval = do_command(opt, cmd.code, &cfg);
	// unmap nvme reg

	cleanup(cfg, opt);

	return retval;
}

void show_usage(void)
{
	printf("%s [command] [-p pci_id | -u usb_dev] [options]\n", _prog);
}

void show_usage_command(uint8_t c)
{
	printf("command is %x\n", c);

	printf("[bist]\n");
	printf(" -g          testcase group (run)\n");
	printf(" -n          testcase number (run)\n");
	printf(" --get-info  get bist info\n");
	printf(" --get-tc    get bist tc\n");

	printf("example:\n");
	printf(" etcboot nand-bist --get-info\n");
	printf(" etcboot dram-bist --get-tc\n");
	printf(" etcboot peri-bist -g 0 -n 1\n");
}
