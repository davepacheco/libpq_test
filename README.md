# libpq_test

This repo has a tiny C program that uses libpq, plus a Makefile that attempts to build the C program three times: once using standard dynamic linking to libpq, once using static linking to libpq, and once using static linking with the `--whole-archive` linker option.

## Dynamically-linked to libpq

```
$ make do_test_dynamic
gcc -o do_test_dynamic -Wall -Werror -I"/opt/ooce/pgsql-13/include" do_test.c -L"/opt/ooce/pgsql-13/lib/amd64" -R"/opt/ooce/pgsql-13/lib/amd64" -lpq
$ file do_test_dynamic
do_test_dynamic:	ELF 64-bit LSB executable AMD64 Version 1, dynamically linked, not stripped, no debugging information available
$ ldd do_test_dynamic
	libpq.so.5 =>	 /opt/ooce/pgsql-13/lib/amd64/libpq.so.5
	libc.so.1 =>	 /lib/64/libc.so.1
	libssl.so.1.1 =>	 /lib/64/libssl.so.1.1
	libcrypto.so.1.1 =>	 /lib/64/libcrypto.so.1.1
	libnsl.so.1 =>	 /lib/64/libnsl.so.1
	libsocket.so.1 =>	 /lib/64/libsocket.so.1
	libm.so.2 =>	 /lib/64/libm.so.2
	libz.so =>	 /lib/64/libz.so
	libmp.so.2 =>	 /lib/64/libmp.so.2
	libmd.so.1 =>	 /lib/64/libmd.so.1
$ ./do_test_dynamic
PQlibVersion=130000
$
```

## Statically-linked to libpq

[After PostgreSQL 12, this requires linking to libpgcommon and libpgport as
well.](https://www.postgresql.org/message-id/CACS8yHL0gbL3ECONrDygcBioTQBY%3DoVG-KGWB6%2BN7spG%2BeSMQw%40mail.gmail.com)

```
$ make do_test_static
gcc -o do_test_static -Wall -Werror -I"/opt/ooce/pgsql-13/include" do_test.c -L"/opt/ooce/pgsql-13/lib/amd64" -Wl,-Bstatic -lpq -lpgcommon -lpgport -Wl,-Bdynamic -lssl -lcrypto -lsocket
$ file do_test_static
do_test_static:	ELF 64-bit LSB executable AMD64 Version 1, dynamically linked, not stripped, no debugging information available
$ ldd ./do_test_static
	libssl.so.1.1 =>	 /lib/64/libssl.so.1.1
	libcrypto.so.1.1 =>	 /lib/64/libcrypto.so.1.1
	libsocket.so.1 =>	 /lib/64/libsocket.so.1
	libc.so.1 =>	 /lib/64/libc.so.1
	libz.so =>	 /lib/64/libz.so
	libnsl.so.1 =>	 /lib/64/libnsl.so.1
	libmp.so.2 =>	 /lib/64/libmp.so.2
	libmd.so.1 =>	 /lib/64/libmd.so.1
	libm.so.2 =>	 /lib/64/libm.so.2
$ ./do_test_static
PQlibVersion=130000
```

## Statically-linked to libpq with --whole-archive option

```
$ make do_test_static_wholearchive
gcc -o do_test_static_wholearchive -Wall -Werror -I"/opt/ooce/pgsql-13/include" do_test.c -L"/opt/ooce/pgsql-13/lib/amd64" -Wl,-Bstatic -Wl,--whole-archive -lpq -lpgcommon -lpgport -Wl,--no-whole-archive -Wl,-Bdynamic -lssl -lcrypto -lsocket
ld: fatal: symbol 'pqsignal' is multiply-defined:
	(file /opt/ooce/pgsql-13/lib/amd64/libpq.a(legacy-pqsignal.o) type=FUNC; file /opt/ooce/pgsql-13/lib/amd64/libpgport.a(pqsignal.o) type=FUNC);
ld: fatal: file processing errors. No output written to do_test_static_wholearchive
collect2: error: ld returned 1 exit status
make: *** [GNUmakefile:17: do_test_static_wholearchive] Error 1
```

Sure enough, those two .a files each have a symbol called "pqsignal", and they are not the same.

## Test system

```
$ uname -a
SunOS epsilon3 5.11 helios-1.0.20664 i86pc i386 i86pc illumos
```

```
$ make --version
GNU Make 4.3
Built for x86_64-pc-solaris2.11
Copyright (C) 1988-2020 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
```

```
$ gcc --version
gcc (Helios 1/10.2.0-il-0) 10.2.0
Copyright (C) 2020 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

```
$ pg_config
BINDIR = /opt/ooce/pgsql-13/bin
DOCDIR = /opt/ooce/pgsql-13/share/doc
HTMLDIR = /opt/ooce/pgsql-13/share/doc
INCLUDEDIR = /opt/ooce/pgsql-13/include
PKGINCLUDEDIR = /opt/ooce/pgsql-13/include
INCLUDEDIR-SERVER = /opt/ooce/pgsql-13/include/server
LIBDIR = /opt/ooce/pgsql-13/lib/amd64
PKGLIBDIR = /opt/ooce/pgsql-13/lib/amd64
LOCALEDIR = /opt/ooce/pgsql-13/share/locale
MANDIR = /opt/ooce/pgsql-13/share/man
SHAREDIR = /opt/ooce/pgsql-13/share
SYSCONFDIR = /etc/opt/ooce/pgsql-13
PGXS = /opt/ooce/pgsql-13/lib/amd64/pgxs/src/makefiles/pgxs.mk
CONFIGURE =  '--includedir=/opt/ooce/pgsql-13/include' '--bindir=/opt/ooce/pgsql-13/bin/amd64' '--sbindir=/opt/ooce/pgsql-13/sbin/amd64' '--libdir=/opt/ooce/pgsql-13/lib/amd64' '--libexecdir=/opt/ooce/pgsql-13/libexec/amd64' '--bindir=/opt/ooce/pgsql-13/bin' '--enable-dtrace' 'DTRACEFLAGS=-64' '--prefix=/opt/ooce/pgsql-13' '--sysconfdir=/etc/opt/ooce/pgsql-13' '--localstatedir=/var/opt/ooce/pgsql-13' '--enable-thread-safety' '--with-openssl' '--with-libxml' '--with-xslt' '--with-readline' '--without-systemd' '--with-system-tzdata=/usr/share/lib/zoneinfo' 'CC=gcc' 'CFLAGS= -O2  -fno-aggressive-loop-optimizations -O3 -m64' 'LDFLAGS= -m64' 'CPPFLAGS= ' 'CXX=g++' 'CXXFLAGS= -O2  -fno-aggressive-loop-optimizations -m64' 'PKG_CONFIG_PATH=/opt/ooce/lib/amd64/pkgconfig'
CC = gcc
CPPFLAGS = -I/usr/include/libxml2
CFLAGS = -Wall -Wmissing-prototypes -Wpointer-arith -Wdeclaration-after-statement -Werror=vla -Wendif-labels -Wmissing-format-attribute -Wimplicit-fallthrough=3 -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -Wno-format-truncation -Wno-stringop-truncation  -O2  -fno-aggressive-loop-optimizations -O3 -m64
CFLAGS_SL = -fPIC
LDFLAGS = -m64 -L/usr/lib/amd64 -Wl,-R'/opt/ooce/pgsql-13/lib/amd64'
LDFLAGS_EX =
LDFLAGS_SL =
LIBS = -lpgcommon -lpgport -lxml2 -lssl -lcrypto -lz -lreadline -ltermcap -lnsl -lsocket -lm
VERSION = PostgreSQL 13.0
```
