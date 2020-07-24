#include <stdio.h>

#include "pciecmd.h"

/* opcode */
#define EVER_PCIEBOOT_OPC_STATUS_CHECK    (0x01)
#define EVER_PCIEBOOT_OPC_SET_ADDRESS     (0x02)
#define EVER_PCIEBOOT_OPC_SET_DATA_LEN    (0x03)
#define EVER_PCIEBOOT_OPC_DOWNLOAD_DAT    (0x04)
#define EVER_PCIEBOOT_OPC_JUMP_TO_IMG     (0x05)
/* cplcode */
#define EVER_PCIEBOOT_CPL_OK                                  (0x00)
#define EVER_PCIEBOOT_CPL_ERR_INVAL_OP                        (0x01)
#define EVER_PCIEBOOT_CPL_ERR_MEM_INIT_FAIL                   (0x02)
#define EVER_PCIEBOOT_CPL_ERR_UNACCESSIBLE_AREA               (0x03)
#define EVER_PCIEBOOT_CPL_ERR_DOWNLOAD_DATA_FAIL              (0x04)
#define EVER_PCIEBOOT_CPL_ERR_INVAL_BAUDRATE                  (0x05)
#define EVER_PCIEBOOT_CPL_ERR_INVAL_TXN_SIZE                  (0x06)
#define EVER_PCIEBOOT_CPL_ERR_CRC_MISMATCH                    (0x07)
#define EVER_PCIEBOOT_CPL_ERR_MISMATCH_HDR_CHECKSUM           (0x08)
#define EVER_PCIEBOOT_CPL_ERR_HAS_NO_MASTER_KEY               (0x68)
#define EVER_PCIEBOOT_CPL_ERR_NOT_SAFE_SECURITY_VER           (0x69)
#define EVER_PCIEBOOT_CPL_ERR_1STCERT_LOAD_FAIL               (0x6A)
#define EVER_PCIEBOOT_CPL_ERR_2NDCERT_LOAD_FAIL               (0x6B)
#define EVER_PCIEBOOT_CPL_ERR_IMG_SIGN_LOAD_FAIL              (0x6C)
#define EVER_PCIEBOOT_CPL_ERR_SECURE_IMG_LOAD_FAIL            (0x6D)
#define EVER_PCIEBOOT_CPL_ERR_IMG_MOVE_FAIL                   (0x6E)
#define EVER_PCIEBOOT_CPL_ERR_MISMATCH_IMG_CHECKSUM           (0x6F)
#define EVER_PCIEBOOT_CPL_ERR_LEVEL_VIOLATION                 (0x70)
#define EVER_PCIEBOOT_CPL_ERR_COMMAND_BLOCK                   (0xFF)

uint32_t ever_status(uint8_t subopc)
{
	uint32_t cpl = EVER_PCIEBOOT_CPL_OK;

	return cpl;
}

uint32_t ever_address(uint64_t addr)
{
	uint32_t cpl = EVER_PCIEBOOT_CPL_OK;

	return cpl;

}

uint32_t ever_image(uint8_t *img, uint32_t len)
{
	uint32_t cpl = EVER_PCIEBOOT_CPL_OK;

	return cpl;

}

uint32_t ever_jump(uint64_t jump)
{
	uint32_t cpl = EVER_PCIEBOOT_CPL_OK;

	return cpl;

}

uint32_t ever_download(uint8_t *img, uint32_t len, uint64_t addr)
{
	uint32_t cpl = EVER_PCIEBOOT_CPL_OK;

	return cpl;
}

uint32_t ever_run(uint8_t *img, uint32_t len, uint64_t addr)
{
	uint32_t cpl = EVER_PCIEBOOT_CPL_OK;

	return cpl;
}

pcieboot_command_t everpcie_cmd = {
	.status = ever_status,
	.address = ever_address,
	.image = ever_image,
	.jump = ever_jump,
	.download = ever_download,
	.run = ever_run,
};
