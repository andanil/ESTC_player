BUILD_ROOT = ../../..

OBJS = color.o
DEPS = color.d

include $(BUILD_ROOT)/common_defs.mk
LIBRARY_NAME = color
LIB = libcolor.a

all: $(LIB)

$(LIB): $(OBJS)
	$(MAKE_LIBRARY)

clean:
	@rm -f $(OBJS) $(DEPS) $(LIB)