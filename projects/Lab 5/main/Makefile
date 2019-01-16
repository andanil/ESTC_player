BUILD_ROOT=../../..
OBJS = main.o

LID_DIR = ../libcolor

LIB_DIRS     += $(LID_DIR)
INCLUDE_DIRS += -I$(LID_DIR)
LDLIBS       += -lcolor

BINARY_NAME = main

include $(BUILD_ROOT)/common_defs.mk

.PHONY: $(BINARY_NAME)

all: $(BINARY_NAME)
$(BINARY_NAME): $(BINARY_NAME).bin

clean:
	@rm -f $(OBJS) main.elf main.bin main.elf.map

flash: $(BINARY_NAME)
	$(FLASH_BINARY)
