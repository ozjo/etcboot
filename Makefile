TARGET := etcboot

################################################################################
# Define Global Variables
################################################################################
Q        := @
CC       := gcc
PRJROOT  := $(PWD)
PCIEBOOT := $(PRJROOT)/pcieboot
UARTBOOT := $(PRJROOT)/uartboot
CFLAGS   :=
INCS     :=
SRCS_DIR :=
PCIE_DIR :=
UART_DIR :=
SRCS     :=
OBJS     :=

TARCHIP  :=

################################################################################
# Set Variables
################################################################################
CFLAGS   += -c -std=c99 -Wall
INCS     += -I$(PRJROOT) -I.
SRCS_DIR += $(PRJROOT)
SRCS     += $(wildcard $(PRJROOT)/*.c)
SRCS     += $(wildcard $(PCIEBOOT)/*.c)
SRCS     += $(wildcard $(UARTBOOT)/*.c)

OBJS     += $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))

################################################################################
# Configure
################################################################################
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(Q)echo "[BUILD] $(notdir $@)"
	$(Q)$(CC) -o $(TARGET) $(notdir $(OBJS))

%.o : %.c
	$(Q)echo "[CC]    $(notdir $@)"
	$(Q)$(CC) $(CFLAGS) $(INCS) $<

clean:
	rm -rf $(TARGET) $(notdir $(OBJS))
