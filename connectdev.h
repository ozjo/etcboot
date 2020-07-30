#ifndef __CONNECTDEV__
#define __CONNECTDEV__

#include "etcboot.h"

#define PCI_BUS_PREFIX         ":"
#define PCI_DEV_PREFIX         "."
#define PCI_FUNC_PREFIX        " "

#define PCI_BUS_MAX            (0xFF)
#define PCI_DEV_MAX            (0xFF)
#define PCI_FUNC_MAX           (0xF)

#define PCI_FUNC_BITOFF        (0)
#define PCI_DEV_BITOFF         (4)
#define PCI_BUS_BITOFF         (12)

#define PCI_VENID_FADU         (0x1dc5)
#define PCI_DEVID_ANNA         (0x6150)
#define PCI_DEVID_EVER         (0x4081)

#define INVAL_PCIPATH          (INT32_MAX)

int connect_device(char *devname, bootcfg_t *b);

#endif /* __CONNECTDEV__ */
