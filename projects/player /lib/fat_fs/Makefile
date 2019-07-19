BUILD_ROOT=../../../..

vpath %.c src
vpath %.c src/option


# Includes
CFLAGS = -Iinc
CFLAGS += -I../usb/inc

# Sources
SRCS = ff.c fattime.c ccsbcs.c 

OBJS = $(SRCS:.c=.o)

include $(BUILD_ROOT)/common_defs.mk

LIBRARY_NAME = fatfs
LIB = libfatfs.a

all: $(LIB)

$(LIB): $(OBJS)
	$(MAKE_LIBRARY)

clean:
	rm -f $(OBJS) $(LIB)
	