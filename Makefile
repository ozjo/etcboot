TARGET := etcboot

##################################################
# Define Global Variables
##################################################
Q        := @
CC       := gcc
PRJROOT  := .
CFLAGS   :=
INCS     :=
SRCS     :=
SRCS_DIR :=
OBJS     :=

##################################################
# Set Variables
##################################################
CFLAGS   += -c -std=c99
INCS     += -I.
SCRS_DIR += $(PRJROOT)
SRCS     += $(wildcard $(SCRS_DIR)/*.c)
OBJS     += $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))


all: $(TARGET)

$(TARGET): $(OBJS)
	$(Q)echo "[BUILD] $(notdir $@)"
	$(Q)$(CC) -o $(TARGET) $(OBJS)

$(PRJROOT)/%.o: %.c
	$(Q)echo "[CC]    $(notdir $@)"
	$(Q)$(CC) $(CFLAGS) $(INCS) $<

clean:
	rm -rf $(TARGET) $(OBJS)
