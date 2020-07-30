#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include "etcboot.h"
#include "connectdev.h"

extern uint8_t chip;
extern int errno;

static const char *pcipathlist[] = {
	"/sys/bus/pci/devices/0000:%02x:%02x.%x",
	"/sys/bus/pci/devices/%02x:%02x.%x",
	NULL
};

static const char *usbpathlist[] = {
	"/dev/%s",
	NULL
};

static int get_bdf(char *strbdf, uint32_t *bdf)
{
	char orgbdf[FILENAME_MAX];
	char *tmp;

	strcpy(orgbdf, strbdf);

	tmp = strtok(strbdf, PCI_BUS_PREFIX);
	if (tmp == NULL) {
		goto err;
	}
	*bdf = (uint32_t) ((strtoull(tmp, NULL, 16) & 0xFF) << PCI_BUS_BITOFF);

	tmp = strtok(NULL, PCI_DEV_PREFIX);
	if (tmp == NULL) {
		goto err;
	}
	*bdf |= (uint32_t) ((strtoull(tmp, NULL, 16) & 0xFF) << PCI_DEV_BITOFF);

	tmp = strtok(NULL, PCI_FUNC_PREFIX);
	if (tmp == NULL) {
		goto err;
	}
	*bdf |= (uint32_t) ((strtoull(tmp, NULL, 16) & 0xF) << PCI_FUNC_BITOFF);

	/* remove remain token */
	while (tmp != NULL) {
		tmp = strtok(NULL, PCI_FUNC_PREFIX);
	};

	strcpy(strbdf, orgbdf);

	return EXIT_SUCCESS;

err:
	printf("invalid PCI BDF format (%s)\n", orgbdf);
	printf(" -p BUS:DEVICE.FUNCION\n");
	printf("  example) -p 02:00.0\n");

	return EXIT_FAILURE;
}

static int find_pcidev_path(uint16_t bus, uint16_t dev, uint8_t func)
{
	int i = 0;
	char pcidev_path[FILENAME_MAX];
	struct stat s;

	while (pcipathlist[i]) {
		sprintf(pcidev_path, pcipathlist[i], bus, dev, func);
		if ((stat(pcidev_path, &s) == 0) && S_ISDIR(s.st_mode)) {
			break;
		}

		i++;
	}

	if (pcipathlist[i] == NULL) {
		return INVAL_PCIPATH;
	}

	return i;
}

static uint8_t get_chipver_from_pcidev(char *pcidev)
{
	int retval, fd;
	char pcidev_temp[FILENAME_MAX] = { 0, };
	uint16_t devid, venid;

	sprintf(pcidev_temp, "%s/%s", pcidev, PCI_CONFIG);
	fd = open(pcidev_temp, O_RDWR);

	lseek(fd, 0, SEEK_SET);
	retval = read(fd, &venid, 2);
	if (retval == -1) {
		return NEXT;
	}

	lseek(fd, 2, SEEK_SET);
	retval = read(fd, &devid, 2);
	if (retval == -1) {
		return NEXT;
	}

	close(fd);

	if (venid != PCI_VENID_FADU) {
		return NEXT;
	}

	switch (devid) {
		case PCI_DEVID_ANNA:
			return ANNAPURNA;
		case PCI_DEVID_EVER:
			return EVEREST;
		default:
			break;
	}

	return NEXT;
}

static int find_valid_pcidev(uint32_t bdf, char *pcidev, bool manual_bdf)
{
	int i = 0;
	uint16_t bus = (bdf >> PCI_BUS_BITOFF) & 0xFF;
	uint16_t dev = (bdf >> PCI_DEV_BITOFF) & 0xFF;
	uint8_t func = (bdf >> PCI_FUNC_BITOFF) & 0xF;

	if (manual_bdf) {
		i = find_pcidev_path(bus, dev, func);
		if (i == INVAL_PCIPATH) {
			return EXIT_FAILURE;
		}

		sprintf(pcidev, pcipathlist[i], bus, dev, func);
		goto out;
	}

	do {
		i = find_pcidev_path(bus, dev, func);
		if (i != INVAL_PCIPATH) {
			sprintf(pcidev, pcipathlist[i], bus, dev, func);
			if (get_chipver_from_pcidev(pcidev) != NEXT) {
				break;
			}
		}

		func++;

		if (func > PCI_FUNC_MAX) {
			func = 0;
			dev++;
		}
		if (dev > PCI_DEV_MAX) {
			func = 0;
			dev = 0;
			bus++;
		}
	} while (bus <= PCI_BUS_MAX);

out:
	chip = get_chipver_from_pcidev(pcidev);

	return EXIT_SUCCESS;
}

static int connect_pci_dev(char *strbdf, bootcfg_pci_t *p)
{
	int fd, retval;
	uint16_t pcicfg_cmd;
	uint32_t bdf = 0;
	char pcipath_base[FILENAME_MAX] = { 0, };
	char pcipath_temp[FILENAME_MAX] = { 0, };
	bool manual_bdf = false;

	if (strbdf[0] != 0) {
		manual_bdf = true;

		retval = get_bdf(strbdf, &bdf);
		if (retval) {
			return retval;
		}
	}

	if (find_valid_pcidev(bdf, pcipath_base, manual_bdf) == EXIT_FAILURE) {
		printf("not exist pci device (%s)\n", strbdf);
		return ENOENT;
	}

	sprintf(pcipath_temp, "%s/%s", pcipath_base, PCI_RESOURCE0);
	if ((fd = open(pcipath_temp, O_RDWR)) == -1) {
		printf("open pci resource0 failed (%s)\n", pcipath_temp);
		return ENOENT;
	}

	/* map to PCIBAR0 for nvme register accessing */
	p->nvmereg = mmap(0, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (p->nvmereg == MAP_FAILED) {
		printf("map to nvme register failed\n");
		return EXIT_FAILURE;
	}

	if (chip == EVEREST) {
		sprintf(pcipath_temp, "%s/%s", pcipath_base, PCI_RESOURCE2);
		if ((fd = open(pcipath_temp, O_RDWR)) == -1) {
			printf("open pci resource2 failed (%s)\n", pcipath_temp);
			return ENOENT;
		}

		/* map to PCIBAR2 for mpm accessing */
		p->mpmbase = mmap(0,  MPMSIZE_MAX, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		close(fd);
		if (p->mpmbase == MAP_FAILED) {
			printf("map to nvme register failed\n");
			return EXIT_FAILURE;
		}
	}

	/* memory space enable */
	sprintf(pcipath_temp, "%s/%s", pcipath_base, PCI_CONFIG);
	fd = open(pcipath_temp, O_RDWR);
	lseek(fd, PCICFG_CMD_OFFSET, SEEK_SET);
	if ((retval = read(fd, &pcicfg_cmd, PCICFG_CMD_SIZE)) == -1) {
		printf("read pci configuration register failed\n");
		return errno;
	}

	if ((pcicfg_cmd & PCICFG_CMD_MSE) != PCICFG_CMD_MSE) {
		pcicfg_cmd |= PCICFG_CMD_MSE;
		if ((retval = write(fd, &pcicfg_cmd, PCICFG_CMD_SIZE)) == -1) {
			printf("write pci configuration register failed\n");
			return errno;
		}
	}
	close(fd);

	return EXIT_SUCCESS;
}

static int connect_uart_dev(char *path, bootcfg_uart_t *u)
{
#if 0
	sprintf(path, usbpathlist[i], o.dev);
	if ((fd = open(path, O_RDWR)) != -1) {
		cfg->fd[0] = fd;
	}
#endif

	return EXIT_SUCCESS;
}

int connect_device(char *devname, bootcfg_t *b)
{
	int retval;

	switch (b->target) {
		case PCIEBOOT:
			retval = connect_pci_dev(devname, &b->pci);
			break;
		case UARTBOOT:
			retval = connect_uart_dev(devname, &b->uart);
			break;
		default:
			retval = ENXIO;
	}


	return retval;
}
