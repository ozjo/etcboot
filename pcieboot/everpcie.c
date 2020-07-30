#include <stdio.h>
#include <stdlib.h>

#include "pciecmd.h"

/* opcode */
#define EVER_PCIEBOOT_OPC_STATUS_CHECK    (0x1)
#define EVER_PCIEBOOT_OPC_SET_ADDRESS     (0x2)
#define EVER_PCIEBOOT_OPC_SET_DATA_LEN    (0x3)
#define EVER_PCIEBOOT_OPC_DOWNLOAD_DAT    (0x4)
#define EVER_PCIEBOOT_OPC_JUMP_TO_IMG     (0x5)
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

int ever_status(uint8_t subopc, uint32_t *st, uint32_t *cpl)
{
	int retval = EXIT_SUCCESS;

	return retval;
}

int ever_address(uint64_t a, uint32_t *c)
{
	int retval = EXIT_SUCCESS;

	return retval;
}

int ever_download(uint8_t *i, uint32_t l, uint32_t *c)
{
	int retval = EXIT_SUCCESS;

	return retval;
}

int ever_run(uint32_t t, uint32_t *c)
{
	int retval = EXIT_SUCCESS;

	return retval;
}

pcieboot_opr_t everpcie_opr = {
	.status = ever_status,
	.address = ever_address,
	.download = ever_download,
	.run = ever_run,
};
