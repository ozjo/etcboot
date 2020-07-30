#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pciecmd.h"

/* opcode */
#define ANNA_PCIEBOOT_OPC_GET_STATUS      (0x0)
#define ANNA_PCIEBOOT_OPC_SET_FW_BUF      (0x1)
#define ANNA_PCIEBOOT_OPC_SET_FW_LEN      (0x2)
#define ANNA_PCIEBOOT_OPC_SEND_FW         (0x3)
#define ANNA_PCIEBOOT_OPC_SET_AEX_TXN     (0x9)
#define ANNA_PCIEBOOT_OPC_JUMP_TO_FW      (0xD)
#define ANNA_PCIEBOOT_OPC_BIST            (0xE)
/* subopc */
#define ANNA_PCIEBOOT_SUBOPC_ADDR_LO      (0x0)
#define ANNA_PCIEBOOT_SUBOPC_ADDR_UP      (0x1)
/* mask */
#define ANNA_PCIEBOOT_MASK_HEIGHT         (0x1)
#define ANNA_PCIEBOOT_MASK_OPCODE         (0xF)
#define ANNA_PCIEBOOT_MASK_SUBOPC         (0xF)
/* bitoff */
#define ANNA_PCIEBOOT_BITOFF_HEIGHT       (0)
#define ANNA_PCIEBOOT_BITOFF_SUBOPC       (24)
#define ANNA_PCIEBOOT_BITOFF_OPCODE       (28)

/* cplcode */
#define ANNA_PCIEBOOT_CPL_OK                                   (0x00)
#define ANNA_PCIEBOOT_CPL_ERR                                  (0x01)
#define ANNA_PCIEBOOT_CPL_ERR_INVALID_SUB_OPCODE               (0x02)
#define ANNA_PCIEBOOT_CPL_ERR_ADDRESS_HIGH_LOW_NOT_SPECIFIED   (0x03)
#define ANNA_PCIEBOOT_CPL_ERR_KEY_TYPE_NOT_SPECIFIED           (0x04)
#define ANNA_PCIEBOOT_CPL_ERR_SHA_CALC_FAIL                    (0x05)
#define ANNA_PCIEBOOT_CPL_ERR_DIGEST_MISMATCH                  (0x06)
#define ANNA_PCIEBOOT_CPL_ERR_INVALID_OPCODE                   (0x07)
#define ANNA_PCIEBOOT_CPL_ERR_INVALID_FW_LEN                   (0x08)
#define ANNA_PCIEBOOT_CPL_ERR_INVALID_FW_ADDR                  (0x09)
#define ANNA_PCIEBOOT_CPL_ERR_INVALID_SECURE_BOOT_KEY          (0x0A)
#define ANNA_PCIEBOOT_CPL_ERR_HMAC_CALC_FAIL                   (0x0B)
#define ANNA_PCIEBOOT_CPL_ERR_AES_TXN_SZ_OUT_OF_RANGE          (0x0C)
#define ANNA_PCIEBOOT_CPL_ERR_DATA_SZ_LESS_THAN_AES_TXN_SZ     (0x0D)
#define ANNA_PCIEBOOT_CPL_ERR_DATA_SZ_NOT_ALIGN_TO_AES_TXN_SZ  (0x0E)
#define ANNA_PCIEBOOT_CPL_ERR_CRYPTO_SELFTEST_FAIL             (0x0F)

#define ANNA_PCIEBOOT_DOWN_OFF_MIN         (0x40)
#define ANNA_PCIEBOOT_DOWN_OFF_MAX         (0xFFFFFF)

static uint32_t get_status(uint32_t subopc, uint32_t *stat)
{
	uint32_t cpl;
	pcieboot_cmd_t cmd = {
		.rom = {
			.opc    = ANNA_PCIEBOOT_OPC_GET_STATUS,
			.subopc = subopc,
			.param  = 0,
		},
	};

	cpl = cmd_procedure_via_nvmereg(cmd.dw, WITHRDATA, stat);
	if (cpl != ANNA_PCIEBOOT_CPL_OK) {
		printf("get status failed (cpl %08x)\n", cpl);
	}

	printf("status %08x\n", *stat);

	return cpl;
}

static uint32_t set_fw_buf(uint64_t addr, bool height)
{
	uint32_t cpl;
	uint32_t addr32 = height ? getaddrhi(addr) : getaddrlo(addr);
	pcieboot_cmd_t cmd = {
		.rom = {
			.opc    = ANNA_PCIEBOOT_OPC_SET_FW_BUF,
			.subopc = 0,
			.param  = height ? ANNA_PCIEBOOT_SUBOPC_ADDR_UP : ANNA_PCIEBOOT_SUBOPC_ADDR_LO,
		},
	};


	cpl = cmd_procedure_via_nvmereg(cmd.dw, WITHWDATA, &addr32);
	if (cpl != ANNA_PCIEBOOT_CPL_OK) {
		printf("set fw buffer (%s) failed (cpl %08x)\n", height ? "upper" : "lower", cpl);
	}

	return cpl;
}

static uint32_t set_fw_len(uint32_t len)
{
	uint32_t cpl;
	pcieboot_cmd_t cmd = {
		.rom = {
			.opc    = ANNA_PCIEBOOT_OPC_SET_FW_LEN,
			.subopc = 0,
			.param  = 0,
		},
	};

	cpl = cmd_procedure_via_nvmereg(cmd.dw, WITHWDATA, &len);
	if (cpl != ANNA_PCIEBOOT_CPL_OK) {
		printf("set fw length failed (cpl %08x)\n", cpl);
	}

	return cpl;
}

static uint32_t send_fw(uint32_t fw, uint32_t off)
{
	uint32_t cpl;
	pcieboot_cmd_t cmd = {
		.rom = {
			.opc    = ANNA_PCIEBOOT_OPC_SEND_FW,
			.subopc = 0,
			.param  = off,
		},
	};

	cpl = cmd_procedure_via_nvmereg(cmd.dw, WITHWDATA, &fw);
	if (cpl != ANNA_PCIEBOOT_CPL_OK) {
		printf("send fw failed (cpl %08x)\n", cpl);
	}

	return cpl;
}

static uint32_t set_aes_txn_size(uint32_t sz)
{
	uint32_t cpl;
	pcieboot_cmd_t cmd = {
		.rom = {
			.opc    = ANNA_PCIEBOOT_OPC_SET_AEX_TXN,
			.subopc = 0,
			.param  = 0,
		},
	};

	cpl = cmd_procedure_via_nvmereg(cmd.dw, WITHWDATA, &sz);
	if (cpl != ANNA_PCIEBOOT_CPL_OK) {
		printf("set aes transaction size failed (cpl %08x)\n", cpl);
	}

	return cpl;
}

static uint32_t jump_to_fw(void)
{
	uint32_t cpl;
	pcieboot_cmd_t cmd = {
		.rom = {
			.opc    = ANNA_PCIEBOOT_OPC_JUMP_TO_FW,
			.subopc = 0,
			.param  = 0,
		},
	};

	cpl = cmd_procedure_via_nvmereg(cmd.dw, WITHNODATA, NULL);
	if (cpl != ANNA_PCIEBOOT_CPL_OK) {
		printf("set aes transaction size failed (cpl %08x)\n", cpl);
	}

	return cpl;
}

static uint32_t bist(uint8_t type, uint8_t bcmd, uint8_t tcgrp, uint8_t tcnum)
{
	uint32_t cpl;
	pcieboot_cmd_t cmd = {
		.bist = {
			.opc = ANNA_PCIEBOOT_OPC_BIST,
			.type = type,
			.bcmd = bcmd,
			.tcgrp = tcgrp,
			.tcnum = tcnum,
		},
	};

	cpl = cmd_procedure_via_nvmereg(cmd.dw, WITHNODATA, NULL);

	return cpl;
}

int anna_status(uint8_t sub, uint32_t *st, uint32_t *c)
{
	int retval = EXIT_SUCCESS;

	*c = get_status(sub, st);
	if (*c != ANNA_PCIEBOOT_CPL_OK) {
		retval = EXIT_FAILURE;
	}

	return retval;
}

int anna_address(uint64_t a, uint32_t *c)
{
	int retval = EXIT_SUCCESS;

	for (int h = ADDR_LO; h <= ADDR_HI; h++) {
		*c = set_fw_buf(a, h);
		if (*c != ANNA_PCIEBOOT_CPL_OK) {
			retval = EXIT_FAILURE;
			break;
		}
	}

	return retval;
}

int anna_download(uint8_t *i, uint32_t l, uint32_t *c)
{
	int retval = EXIT_SUCCESS;

	if (l <= ANNA_PCIEBOOT_DOWN_OFF_MIN) {
		printf("download offset should not be less than 64B\n");
		retval = EXIT_FAILURE;
		goto out;
	}

	if (l > ANNA_PCIEBOOT_DOWN_OFF_MAX) {
		printf("download offset should not be larger than 16MB\n");
		retval = EXIT_FAILURE;
		goto out;
	}

	for (int o = 0; o < (l / 4); o++) {
		*c = send_fw((*((uint32_t *) i + o)), o * 4);
		if (*c != ANNA_PCIEBOOT_CPL_OK) {
			retval = EXIT_FAILURE;
			break;
		}
	}

	*c = set_fw_len(l);
	if (*c != ANNA_PCIEBOOT_CPL_OK) {
		retval = EXIT_FAILURE;
	}

out:
	return retval;
}

int anna_run(uint32_t aes_txn_sz, uint32_t *c)
{
	int retval = EXIT_SUCCESS;

	*c = set_aes_txn_size((uint32_t) aes_txn_sz);
	if (*c != ANNA_PCIEBOOT_CPL_OK) {
		retval = EXIT_FAILURE;
		goto out;
	}

	*c = jump_to_fw();
	if (*c != ANNA_PCIEBOOT_CPL_OK) {
		retval = EXIT_FAILURE;
	}

out:
	return retval;
}

int anna_bist(uint8_t t, uint8_t b, uint8_t g, uint8_t n, uint32_t *c)
{
	int retval = EXIT_SUCCESS;

	*c = bist(t, b, g, n);
	if (*c != ANNA_PCIEBOOT_CPL_OK) {
		retval = EXIT_FAILURE;
	}

	return retval;
}

pcieboot_opr_t annapcie_opr = {
	.status = anna_status,
	.address = anna_address,
	.download = anna_download,
	.run = anna_run,
	.bist = anna_bist,
};
