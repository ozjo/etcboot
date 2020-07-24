#include <stdio.h>

#include "pciecmd.h"

/* opcode */
#define ANNA_PCIEBOOT_OPC_GET_STATUS      (0x00)
#define ANNA_PCIEBOOT_OPC_SET_FW_BUF      (0x01)
#define ANNA_PCIEBOOT_OPC_SET_FW_LEN      (0x02)
#define ANNA_PCIEBOOT_OPC_SEND_FW         (0x03)
#define ANNA_PCIEBOOT_OPC_SET_AEX_TXN     (0x09)
#define ANNA_PCIEBOOT_OPC_JUMP_TO_FW      (0x0D)
/* subopc */
#define ANNA_PCIEBOOT_SUBOPC_ADDR_UP      (0x0)
#define ANNA_PCIEBOOT_SUBOPC_ADDR_LO      (0x1)
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

static uint32_t set_fw_buffer(uint64_t addr)
{
	uint32_t cpl;
	uint32_t cmd = ANNA_PCIEBOOT_OPC_SET_FW_BUF;

	send_data((uint32_t) (addr & 0xFFFFFFFF));
	send_command(cmd | ANNA_PCIEBOOT_SUBOPC_ADDR_LO);
	cpl = recv_completion();

	if (cpl != ANNA_PCIEBOOT_CPL_OK) {
		printf("set fw buffer (lower) failed\n");
		return cpl;
	}

	send_data((uint32_t) ((addr >> 32) & 0xFFFFFFFF));
	send_command(cmd | ANNA_PCIEBOOT_SUBOPC_ADDR_UP);
	cpl = recv_completion();

	if (cpl != ANNA_PCIEBOOT_CPL_OK) {
		printf("set fw buffer (upper) failed\n");
	}

	return cpl;
}

uint32_t anna_status(uint8_t subopc)
{
	uint32_t cpl = ANNA_PCIEBOOT_CPL_OK;

	return cpl;
}

uint32_t anna_address(uint64_t addr)
{
	uint32_t cpl = ANNA_PCIEBOOT_CPL_OK;

	return cpl;

}

uint32_t anna_image(uint8_t *img, uint32_t len)
{
	uint32_t cpl = ANNA_PCIEBOOT_CPL_OK;

	return cpl;

}

uint32_t anna_jump(uint64_t jump)
{
	uint32_t cpl = ANNA_PCIEBOOT_CPL_OK;

	return cpl;

}

uint32_t anna_download(uint8_t *img, uint32_t len, uint64_t addr)
{
	uint32_t cpl = ANNA_PCIEBOOT_CPL_OK;

	return cpl;
}

uint32_t anna_run(uint8_t *img, uint32_t len, uint64_t addr)
{
	uint32_t cpl = ANNA_PCIEBOOT_CPL_OK;

	return cpl;
}

pcieboot_command_t annapcie_cmd = {
	.status = anna_status,
	.address = anna_address,
	.image = anna_image,
	.jump = anna_jump,
	.download = anna_download,
	.run = anna_run,
};
