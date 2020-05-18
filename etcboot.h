#ifndef __ETCBOOT__
#define __ETCBOOT__

#define ANNAPURNA       0
#define EVEREST         1

#define CMD_ADDRESS     0
#define CMD_IMAGE       1
#define CMD_JUMP        2
#define CMD_DOWNLOAD    3
#define CMD_RUN         4

#define NOTSUPBOOT      0
#define PCIEBOOT        1
#define UARTBOOT        2

typedef struct bootcfg {
	uint64_t addr;
	uint8_t *fw;
	uint32_t len;
} bootcfg_t;

#endif /* __ETCBOOT__ */
