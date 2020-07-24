TARGET := etcboot

################################################################################
# Define Global Variables
################################################################################
Q        := @
CC       := gcc
PRJROOT  := .
PCIEBOOT := $(PRJROOT)/pcieboot
UARTBOOT := $(PRJROOT)/uartboot
CFLAGS   :=
INCS     :=
SRCS     :=
SRCS_DIR :=
OBJS     :=

################################################################################
# Set Variables
################################################################################
CFLAGS   += -c -std=c99 -Wall
INCS     += -I.
SCRS_DIR += $(PRJROOT)
SRCS     += $(wildcard $(SCRS_DIR)/*.c)
OBJS     += $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))

################################################################################
# Configure
################################################################################
TARCHIP  :=


all: $(TARGET)

$(TARGET): $(OBJS)
	$(Q)echo "[BUILD] $(notdir $@)"
	$(Q)$(CC) -o $(TARGET) $(OBJS)

$(PRJROOT)/%.o: $(PRJROOT)/%.c
	$(Q)echo "[CC]    $(notdir $@)"
	$(Q)$(CC) $(CFLAGS) $(INCS) $<

$(PRJROOT)/%.o: $(UARTBOOT)/%.c
	$(Q)echo "[CC]    $(notdir $@)"
	$(Q)$(CC) $(CFLAGS) $(INCS) $<

$(PRJROOT)/%.o: $(PCIEBOOT)/%.c
	$(Q)echo "[CC]    $(notdir $@)"
	$(Q)$(CC) $(CFLAGS) $(INCS) $<

clean:
	rm -rf $(TARGET) $(OBJS)
