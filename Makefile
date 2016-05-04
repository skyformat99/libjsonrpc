program_NAME := libjsonrpc
program_BUILD_DIR := build
program_C_SRCS := $(wildcard src/*.c)
program_C_OBJS :=$(program_C_SRCS:.c=.o)
program_INCLUDE_DIRS :=
program_LIBRARY_DIRS :=
program_LIBRARIES := jansson curl m jsonrpc

program_C_TESTS := $(wildcard tests/*.c)

program_OPTIMIZATION := -O2
program_STD := -std=c99 -pedantic
program_WARN := -Wall -W -Wextra

CFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
CFLAGS += -c -fpic $(program_OPTIMIZATION) $(program_STD) $(program_WARN)
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all check clean distclean $(program_NAME)

all: builddir $(program_NAME)

$(program_NAME): $(program_C_OBJS)
	$(CC) -shared $(LDFLAGS) $(program_C_OBJS) -o $(program_BUILD_DIR)/$(program_NAME).so

# check: $(program_C_OBJS) $(program_C_TESTS) | builddir
# 	$(CC) $(LDFLAGS) $^ -o $(program_BUILD_DIR)/test
# 	build/test
check: $(program_C_OBJS) $(program_C_TESTS) | builddir
	$(CC) $(LDFLAGS) -L/usr/local/lib/ $(program_C_TESTS) -o $(program_BUILD_DIR)/test
	build/test

builddir:
	@mkdir -p build

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_C_OBJS)
	@- $(RM) -r build bin dist test

distclean: clean

define OBJECT_DEPENDS_ON_CORRESPONDING_HEADER
    $(1) : ${1:.o=.h}
endef

$(foreach object_file,$(program_OBJS),$(eval $(call OBJECT_DEPENDS_ON_CORRESPONDING_HEADER,$(object_file))))
