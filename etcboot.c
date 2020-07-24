#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include "etcboot.h"
#include "pciecmd.h"

const char *_prog = "etcboot";

#if (CHIP == EVEREST)
uint8_t chip = EVEREST;
#else
uint8_t chip = ANNAPURNA;
#endif

extern int errno;

typedef struct options {
	uint8_t bootmode;
	char dev[FILENAME_MAX];
} opt_t;

const char *commands[] = {
	"address",
	"image",
	"jump",
	"download",
	"run"
};

static const char *pcipathlist[] = {
	"/sys/bus/pci/devices/0000:%s",
	"/sys/bus/pci/devices/%s",
	NULL
};

static const char *usbpathlist[] = {
	"/dev/%s",
	NULL
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

	return NOTCMD;
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

	while ((opt = getopt_long(c, v, optlist, optlist_long, NULL)) != -1) {
		switch (opt) {
			case 'A':
				chip = ANNAPURNA;
				break;

			case 'E':
				chip = EVEREST;
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
		printf("device was not entered.\n");
		show_usage();
		retval = EXIT_FAILURE;
	}

out:
	return retval;
}

int find_valid_pcidev(char *bdf, char *pcidev)
{
	int i = 0;
	struct stat s;

	do {
		sprintf(pcidev, pcipathlist[i], bdf);

		if ((stat(pcidev, &s) == 0) && S_ISDIR(s.st_mode)) {
			return EXIT_SUCCESS;
		}
	} while (pcipathlist[i++] == NULL);

	return EXIT_FAILURE;
}

static int connect_pci_dev(char *bdf, bootcfg_pci_t *p)
{
	int fd, retval;
	char pcipath_base[FILENAME_MAX] = { 0, };
	char pcipath_temp[FILENAME_MAX] = { 0, };
	uint16_t pcicfg_cmd;

	if (find_valid_pcidev(bdf, pcipath_base) == EXIT_FAILURE) {
		printf("not exist pci device (%s)\n", pcipath_base);
		return ENOENT;
	}

	sprintf(pcipath_temp, "%s/%s", pcipath_base, PCI_RESOURCE0);
	if ((fd = open(pcipath_temp, O_RDWR)) == -1) {
		printf("open pci resource0 failed (%s)\n", pcipath_temp);
		return ENOENT;
	}

	/* map to PCIBAR0 for nvme register accessing */
	p->nvmereg = (volatile uint32_t *) mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	close(fd);

	if (chip == EVEREST) {
		sprintf(pcipath_temp, "%s/%s", pcipath_base, PCI_RESOURCE2);
		if ((fd = open(pcipath_temp, O_RDWR)) == -1) {
			printf("open pci resource2 failed (%s)\n", pcipath_temp);
			return ENOENT;
		}

		/* map to PCIBAR2 for mpm accessing */
		p->mpmbase = (volatile uint32_t *) mmap(NULL,  MPMSIZE_MAX, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
		close(fd);
	}

	/* memory space enable */
	sprintf(pcipath_temp, "%s/%s", pcipath_base, PCI_CONFIG);
	fd = open(pcipath_temp, O_RDWR);
	if ((retval = pread(fd, &pcicfg_cmd, PCICFG_COMMAND_SIZE, PCICFG_COMMAND_OFFSET)) == -1) {
		printf("read pci configuration register failed\n");
		return errno;
	}

	if ((pcicfg_cmd & PCICFG_COMMAND_MSE) != PCICFG_COMMAND_MSE) {
		pcicfg_cmd |= PCICFG_COMMAND_MSE;
		if ((retval = pwrite(fd, &pcicfg_cmd, PCICFG_COMMAND_SIZE, PCICFG_COMMAND_OFFSET)) == -1) {
			printf("write pci configuration register failed\n");
			return errno;
		}
	}
	close(fd);

	return EXIT_SUCCESS;
}

static int connect_uart_dev(char *d, bootcfg_uart_t *u)
{
#if 0
	sprintf(path, usbpathlist[i], o.dev);
	if ((fd = open(path, O_RDWR)) != -1) {
		cfg->fd[0] = fd;
	}
#endif

	return EXIT_SUCCESS;
}

static int connect_device(char *devname, uint8_t bootmode, bootcfg_t *cfg)
{
	int retval;

	switch (bootmode) {
		case PCIEBOOT:
			retval = connect_pci_dev(devname, &cfg->pci);
			break;
		case UARTBOOT:
			retval = connect_uart_dev(devname, &cfg->uart);
			break;
		default:
			retval = ENXIO;
	}


	return retval;
}

int do_command(uint8_t bootmode, uint8_t cmd, bootcfg_t c)
{
	if (bootmode == PCIEBOOT) {
		return do_pciecmd(cmd, c);
	} else if (bootmode == UARTBOOT) {
		return do_uartcmd(cmd, c);
	} else {
		return ENODEV;
	}
}

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
	if (cmd == NOTCMD) {
		show_usage();

		return EXIT_SUCCESS;
	}

	retval = parse_options(argc, argv, &opt, &cfg);
	if (retval) {
		printf("Parse options failed\n");

		return retval;
	}

	retval = connect_device(opt.dev, opt.bootmode, &cfg);
	if (retval) {
		printf("Connect device failed\n");

		return retval;
	};

	retval = do_command(cmd, opt.bootmode, cfg);
	// unmap nvme reg

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
