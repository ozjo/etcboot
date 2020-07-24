#ifndef __PCIECMD__
#define __PCIECMD__

#include "etcboot.h"

/* Annapurna */
#define ANNA_PCIEBOOT_NVME_REG_BASE       (0x54)
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

typedef struct pcieboot_command {
	uint32_t (*status)(uint8_t subopc);
	uint32_t (*address)(uint64_t addr);
	uint32_t (*image)(uint8_t *img, uint32_t len);
	uint32_t (*jump)(uint64_t addr);
	uint32_t (*download)(uint8_t *img, uint32_t len, uint64_t addr);
	uint32_t (*run)(void);
} pcieboot_command_t;

int do_pciecmd(uint8_t cmd, bootcfg_t cfg);

#endif /* __PCIECMD__ */
