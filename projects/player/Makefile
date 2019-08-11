# Sources
SRCS = main.c actions_indication.c player_internal.c player.c usbh_usr.c song_list.c

BUILD_ROOT=../..

vpath %.c src
vpath %.a lib

# Includes
CFLAGS = -Iinc 

# Library paths
LIBPATHS = lib/usb
LIBPATHS1 = lib/fat_fs

# Libraries to link
LIB_DIRS     += $(LIBPATHS)
INCLUDE_DIRS += -I$(LIBPATHS)
LIB_DIRS     += $(LIBPATHS1)
INCLUDE_DIRS += -I$(LIBPATHS1)
LDLIBS       += -lfatfs -lusb

# Extra includes
CFLAGS += -Ilib/usb/inc
CFLAGS += -Ilib/fat_fs/inc

OBJS = $(SRCS:.c=.o)

BINARY_NAME = main

include $(BUILD_ROOT)/common_defs.mk

.PHONY: $(BINARY_NAME) lib

all: $(BINARY_NAME)

$(BINARY_NAME): lib $(BINARY_NAME).bin 

lib:
	$(MAKE) -C lib all

flash: $(BINARY_NAME)
	$(FLASH_BINARY)

clean:
	rm -f *.o
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin
	$(MAKE) clean -C lib # Remove this line if you don't want to clean the libs as well
	
