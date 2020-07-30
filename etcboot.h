#ifndef __ETCBOOT__
#define __ETCBOOT__

#include <stdint.h>
#include <stdbool.h>

/* chip versions */
#define ANNAPURNA                (0)
#define EVEREST                  (1)
#define NEXT                     (2)

/* etc boot command */
#define NOTCMD           (UINT8_MAX)
#define CMD_STATUS               (0)
#define CMD_ADDRESS              (1)
#define CMD_DOWNLOAD             (2)
#define CMD_RUN                  (3)
#define CMD_PERI_BIST            (4)
#define CMD_DRAM_BIST            (5)
#define CMD_NAND_BIST            (6)

/* bist type */
#define BISTT_PERI               (1)
#define BISTT_DRAM               (2)
#define BISTT_NAND               (3)

#define BISTCMD_RUNTC            (0)
#define BISTCMD_GETBISTINFO      (1)
#define BISTCMD_GETTC            (2)

/* boot modes */
#define PCIEBOOT                 (0)
#define UARTBOOT                 (1)
#define NOTSUPBOOT       (UINT8_MAX)

/* pci configuration */
#define PCICFG_CMD_OFFSET     (0x04)
#define PCICFG_CMD_SIZE          (2)
#define PCICFG_CMD_MSE    (0x1 << 1)

#define PCI_RESOURCE0     "resource0"
#define PCI_RESOURCE2     "resource2"
#define PCI_CONFIG           "config"

/* sizes */
#define MPMSIZE_MAX       (0x1 << 23)

#define INVAL_ADDR        (UINT64_MAX)
#define INVAL_SUBOPC       (UINT8_MAX)
#define INVAL_TCNUM        (UINT8_MAX)
#define INVAL_TCGRP        (UINT8_MAX)

typedef struct bootcfg_pci {
	void *nvmereg;
	void *mpmbase;
} bootcfg_pci_t;

typedef struct bootcfg_uart {
	int fd;
} bootcfg_uart_t;

typedef struct bootcfg {
	uint32_t dat;
	uint32_t cpl;
	uint8_t target;

	uint64_t addr;
	uint8_t *fw;
	uint32_t len;
	uint32_t aes_txn_sz;
	uint8_t subopc;
	bool wrun;
	uint8_t bistcmd;
	uint8_t tcgrp;
	uint8_t tcnum;

	union {
		bootcfg_pci_t pci;
		bootcfg_uart_t uart;
	};
} bootcfg_t;

#endif /* __ETCBOOT__ */
