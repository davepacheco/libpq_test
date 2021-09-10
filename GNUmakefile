#
# This Makefile builds "do_test", a C program that makes minimal use of libpq.
# We build it three ways:
#
# (1) "do_test_dynamic": standard, dynamically-linked to libpq
# (2) "do_test_static": statically-linked to libpq
#     (which also requires linking to libpgcommon and libpgport)
# (3) "do_test_static_wholearchive": like "do_test_static", but using the
#     --whole-archive linker option.
#
all: do_test_dynamic do_test_static do_test_static_wholearchive

clean:
	rm -f do_test_dynamic do_test_static do_test_static_wholearchive

do_test_dynamic do_test_static do_test_static_wholearchive: do_test.c
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

CFLAGS += -Wall -Werror -I"$(shell pg_config --includedir)"
LDFLAGS += -L"$(shell pg_config --libdir)"

ifeq ($(shell uname -s), Linux)
	EXTRALIBS += -lpthread
endif
ifeq ($(shell uname -s), SunOS)
	EXTRALIBS += -lssl -lcrypto -lsocket
endif

do_test_dynamic: LDFLAGS += -Wl,-R"$(shell pg_config --libdir)" -lpq

do_test_static: LDFLAGS += \
    -Wl,-Bstatic \
    -lpq \
    -lpgcommon \
    -lpgport \
    -Wl,-Bdynamic \
    $(EXTRALIBS)

do_test_static_wholearchive: LDFLAGS += \
    -Wl,-Bstatic \
    -Wl,--whole-archive \
    -lpq \
    -lpgcommon \
    -lpgport \
    -Wl,--no-whole-archive \
    -Wl,-Bdynamic \
    $(EXTRALIBS)
