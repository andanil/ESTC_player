BUILD_ROOT=../../../..

vpath %.c src

# Includes
CFLAGS = -Iinc -I../../inc -I../fat_fs/inc

# Sources
SRCS = usb_bsp.c usb_core.c usb_dcd.c usb_dcd_int.c usb_hcd.c usb_hcd_int.c
SRCS += usbh_core.c usbh_hcs.c usbh_ioreq.c usbh_msc_bot.c
SRCS += usbh_msc_core.c  usbh_stdreq.c usbh_msc_fatfs.c usb_otg.c

OBJS = $(SRCS:.c=.o)

include $(BUILD_ROOT)/common_defs.mk

LIBRARY_NAME = usb
LIB = libusb.a

all: $(LIB)


$(LIB): $(OBJS)
	$(MAKE_LIBRARY)

clean:
	rm -f $(OBJS) $(LIB)
	