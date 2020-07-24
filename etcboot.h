#ifndef __ETCBOOT__
#define __ETCBOOT__

/* chip versions */
#define ANNAPURNA        (0)
#define EVEREST          (1)

/* etc boot command */
#define NOTCMD   (UINT8_MAX)
#define CMD_ADDRESS      (0)
#define CMD_IMAGE        (1)
#define CMD_JUMP         (2)
#define CMD_DOWNLOAD     (3)
#define CMD_RUN          (4)

/* boot modes */
#define NOTSUPBOOT       (0)
#define PCIEBOOT         (1)
#define UARTBOOT         (2)

/* pci configuration */
#define PCICFG_COMMAND_OFFSET    (0x04)
#define PCICFG_COMMAND_SIZE      (2)
#define PCICFG_COMMAND_MSE       (0x1 << 1)

#define PCI_RESOURCE0            "resource0"
#define PCI_RESOURCE2            "resource2"
#define PCI_CONFIG               "config"

/* sizes */
#define MPMSIZE_MAX              (0x1 << 23)  /* 8MB */

typedef struct bootcfg_pci {
	volatile void *nvmereg;
	volatile void *mpmbase;
} bootcfg_pci_t;

typedef struct bootcfg_uart {
	int fd;
} bootcfg_uart_t;

typedef struct bootcfg {
	uint64_t addr;
	uint8_t *fw;
	uint32_t len;

	union {
		bootcfg_pci_t pci;
		bootcfg_uart_t uart;
	};
} bootcfg_t;

#endif /* __ETCBOOT__ */
