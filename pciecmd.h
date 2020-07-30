#ifndef __PCIECMD__
#define __PCIECMD__

#include <stdbool.h>

#include "etcboot.h"

/* Annapurna */
#define ANNA_PCIEBOOT_NVME_REG_BASE       (0x50)
#define ANNA_PCIEBOOT_CMD_REG             (ANNA_PCIEBOOT_NVME_REG_BASE + 0x00)
#define ANNA_PCIEBOOT_DAT_REG             (ANNA_PCIEBOOT_NVME_REG_BASE + 0x04)
#define ANNA_PCIEBOOT_CPL_REG             (ANNA_PCIEBOOT_NVME_REG_BASE + 0x08)

/* Everest */
/*  - nvme register */
#define EVER_PCIEBOOT_NVME_REG_BASE       (0x7C)
#define EVER_PCIEBOOT_CPL_REG             (EVER_PCIEBOOT_NVME_REG_BASE + 0x00)
#define EVER_PCIEBOOT_CMD_REG             (EVER_PCIEBOOT_NVME_REG_BASE + 0x04)
#define EVER_PCIEBOOT_DAT_REG             (EVER_PCIEBOOT_NVME_REG_BASE + 0x08)

/* Next chips */
#define NEXT_PCIEBOOT_NVME_REG_BASE       (0x00)

#define ADDR_LO                           (0)
#define ADDR_HI                           (1)
#define CLEAR_CPL                         (UINT32_MAX)

#define WITHWDATA                         (1 << 0)
#define WITHRDATA                         (1 << 1)
#define WITHNODATA                        (1 << 2)

#define getaddrlo(a)                      ((uint32_t) (a & UINT32_MAX))
#define getaddrhi(a)                      ((uint32_t) ((a >> 32) & UINT32_MAX))

typedef struct pcieboot_operation {
	int (*status)(uint8_t subopc, uint32_t *status, uint32_t *cpl);
	int (*address)(uint64_t addr, uint32_t *cpl);
	int (*download)(uint8_t *data, uint32_t len, uint32_t *cpl);
	int (*run)(uint32_t dat, uint32_t *cpl);
	int (*bist)(uint8_t type, uint8_t cmd, uint8_t tcgrp, uint8_t tcnum, uint32_t *cpl);
} pcieboot_opr_t;

typedef struct pcieboot_command {
	union {
		uint32_t dw;

		struct {
			uint32_t param        :24;
			uint32_t subopc       :4;
			uint32_t opc          :4;
		} rom;

		struct {
			uint32_t tcnum        :8;
			uint32_t tcgrp        :2;
			uint32_t rsvd        :12;
			uint32_t bcmd         :2;
			uint32_t type         :4;
			uint32_t opc          :4;
		} bist;
	};
} pcieboot_cmd_t;

uint32_t cmd_procedure_via_nvmereg(uint32_t c, uint8_t ud, uint32_t *d);
int do_pciecmd(uint8_t cmd, bootcfg_t *cfg);

#endif /* __PCIECMD__ */
