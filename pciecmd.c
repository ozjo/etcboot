#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "pciecmd.h"

extern uint8_t chip;
extern pcieboot_opr_t annapcie_opr;
extern pcieboot_opr_t everpcie_opr;
extern pcieboot_opr_t nextpcie_opr;

static volatile uint32_t *cmdreg;
static volatile uint32_t *datreg;
static volatile uint32_t *cplreg;
static volatile uint8_t  *mpm;

static uint32_t cmdreg_off;
static uint32_t datreg_off;
static uint32_t cplreg_off;

static void send_command(uint32_t cmd)
{
	*cmdreg = cmd;
	printf("->CMD(%02x) %08x\n", cmdreg_off, cmd);
}

static void send_data_to_nvmereg(uint32_t dat)
{
	*datreg = dat;
	printf("->DAT(%02x) %08x\n", datreg_off, dat);
}

#if 0
static void send_data_to_mpm(uint64_t off, void *dat, uint64_t n)
{
	memcpy((void *) (mpm + off), dat, n);
}
#endif

static uint32_t recv_data_to_nvmereg(void)
{
	uint32_t dat;

	dat = *datreg;
	printf("<-DAT(%02x) %08x\n", datreg_off, dat);

	return dat;
}

static void send_completion(uint32_t cpl)
{
	*cplreg = cpl;
	printf("->CPL(%02x) %08x\n", cplreg_off, cpl);
}

static uint32_t recv_completion(void)
{
	uint32_t cpl = (uint32_t) (*cplreg);

	/* wait for valid completion */
	while (cpl == CLEAR_CPL) {
		cpl = (uint32_t) (*cplreg);
	};

	printf("<-CPL(%02x) %08x\n", cplreg_off, cpl);
	return cpl;
}

uint32_t cmd_procedure_via_nvmereg(uint32_t c, uint8_t ud, uint32_t *d)
{
	uint32_t cpl;

	send_completion(CLEAR_CPL);
	if ((ud & WITHWDATA) == WITHWDATA) {
		send_data_to_nvmereg(*d);
	}
	send_command(c);
	cpl = recv_completion();
	if ((ud & WITHRDATA) == WITHRDATA) {
		*d = recv_data_to_nvmereg();
	}

	return cpl;
}

static void map_access_memory(void *nvmeregbase, volatile uint32_t *mpmbase)
{
	switch (chip) {
		case ANNAPURNA:
			cmdreg_off = ANNA_PCIEBOOT_CMD_REG;
			datreg_off = ANNA_PCIEBOOT_DAT_REG;
			cplreg_off = ANNA_PCIEBOOT_CPL_REG;
			break;
		case EVEREST:
			cmdreg_off = EVER_PCIEBOOT_CMD_REG;
			datreg_off = EVER_PCIEBOOT_DAT_REG;
			cplreg_off = EVER_PCIEBOOT_CPL_REG;
			mpm = (volatile uint8_t*) (mpmbase);
			break;
		default:
			printf("not emplemented\n");
			break;
	}

	cmdreg = (volatile uint32_t *) (nvmeregbase + cmdreg_off);
	datreg = (volatile uint32_t *) (nvmeregbase + datreg_off);
	cplreg = (volatile uint32_t *) (nvmeregbase + cplreg_off);
}

int do_pciecmd(uint8_t cmdcode, bootcfg_t *b)
{
	int retval = EXIT_SUCCESS;
	pcieboot_opr_t opr;

	map_access_memory(b->pci.nvmereg, b->pci.mpmbase);

	if (chip == ANNAPURNA) {
		opr = annapcie_opr;
	} else if (chip == EVEREST) {
		opr = everpcie_opr;
	} else {
		printf("not implemented\n");
		retval = EXIT_FAILURE;
		goto out;
	}

	switch (cmdcode) {
		case CMD_STATUS:
			retval = opr.status(b->subopc, &b->dat, &b->cpl);
			break;

		case CMD_ADDRESS:  /* set fw buffer address */
			retval = opr.address(b->addr, &b->cpl);
			break;

		case CMD_DOWNLOAD: /* send fw */
			if (b->addr != INVAL_ADDR) {
				retval = opr.address(b->addr, &b->cpl);
			}
			retval = opr.download(b->fw, b->len, &b->cpl);
			if (b->wrun) {
				retval = opr.run(b->aes_txn_sz, &b->cpl);
			}
			break;

		case CMD_RUN:      /* jump to fw */
			retval = opr.run(b->aes_txn_sz, &b->cpl);
			break;

		case CMD_PERI_BIST:
			retval = opr.bist(BISTT_PERI, b->bistcmd, b->tcgrp, b->tcnum, &b->cpl);
			break;

		case CMD_DRAM_BIST:
			retval = opr.bist(BISTT_DRAM, b->bistcmd, b->tcgrp, b->tcnum, &b->cpl);
			break;

		case CMD_NAND_BIST:
			retval = opr.bist(BISTT_NAND, b->bistcmd, b->tcgrp, b->tcnum, &b->cpl);
			break;
	}

out:
	return retval;
}
