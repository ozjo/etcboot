#include <stdio.h>
#include <stdint.h>

#include "pciecmd.h"

extern uint8_t chip;
extern pcieboot_command_t annapcie_cmd;
extern pcieboot_command_t everpcie_cmd;
extern pcieboot_command_t nextpcie_cmd;

static volatile uint32_t *cmdreg;
static volatile uint32_t *datreg;
static volatile uint32_t *cplreg;
static volatile uint8_t  *mpm;

static void setreg(volatile uint32_t *nvmeregbase, volatile uint32_t *mpmbase)
{
	switch (chip) {
		case ANNAPURNA:
			cmdreg = (volatile uint32_t *) (nvmeregbase + ANNA_PCIEBOOT_CMD_REG);
			datreg = (volatile uint32_t *) (nvmeregbase + ANNA_PCIEBOOT_DAT_REG);
			cplreg = (volatile uint32_t *) (nvmeregbase + ANNA_PCIEBOOT_CPL_REG);
			break;
		case EVEREST:
			cmdreg = (volatile uint32_t *) (nvmeregbase + EVER_PCIEBOOT_CMD_REG);
			datreg = (volatile uint32_t *) (nvmeregbase + EVER_PCIEBOOT_DAT_REG);
			cplreg = (volatile uint32_t *) (nvmeregbase + EVER_PCIEBOOT_CPL_REG);
			mpm = (volatile uint8_t*) (mpmbase);
			break;
		deafult:
			/* Do Nothing */
			break;
	}
}

static inline void send_command(uint32_t cmd)
{
	*cmdreg = cmd;
}

static inline void send_data(uint32_t dat)
{
	*datreg = dat;
}

static inline uint32_t recv_completion(void)
{
	return (uint32_t) (*cplreg);
}

int do_pciecmd(uint8_t cmdcode, bootcfg_t cfg)
{
	uint32_t cpl = 0;
	pcieboot_command_t command;

	setreg(cfg.pci.nvmereg, cfg.pci.mpmbase);

	if (chip == ANNAPURNA) {
		command = annapcie_cmd;
	} else if (chip == EVEREST) {
		command = everpcie_cmd;
	} else {
		command = nextpcie_cmd;
	}

	switch (cmdcode) {
		case CMD_ADDRESS:
			cpl = command->address(cfg.addr);
			break;
		case CMD_IMAGE:
			cpl = command->image(cfg.addr);
			break;
		case CMD_JUMP:
			cpl = command->jump(cfg.addr);
			break;
		case CMD_DOWNLOAD:
			cpl = command->download();
			break;
		case CMD_RUN:
			cpl = command->run();
			break;
	}

	return cpl;
}
