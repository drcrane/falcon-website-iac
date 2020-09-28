# Dependencies!
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# https://stackoverflow.com/questions/40621451/makefile-automatically-compile-all-c-files-keeping-o-files-in-separate-folde

PKGCONF?=pkgconf

CC=gcc
CFLAGS=-g -Wno-unused-function -Wno-unknown-pragmas --std=c11 -O2 -Wall -D__COMPILE_FOR_LINUX -Isrc -Iinclude
# LDLIBS=-lfcgi -lssl -lcrypto
LDLIBS=-lm -lcurl -Llib -lapp $(EXTRALIBS) `$(PKGCONF) --libs openssl`

SRCDIR=src/
OBJDIR=obj/

APP_DIR=appsrc/
APP_SRC=$(wildcard $(APP_DIR)*.c)
APP=$(patsubst %.c,%,$(APP_SRC))

TEST_DIR=testsrc/
TEST_SRC=$(wildcard $(TEST_DIR)*.c)
TEST=$(patsubst %.c,%,$(TEST_SRC))

LIBAPPOBJ=obj/base64.o obj/timefn.o obj/filesystem.o obj/json.o obj/dynstringhttp.o

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) -c $(CFLAGS) -o $@ $<

lib/libapp.a : $(LIBAPPOBJ)
	ar -rcs $@ $(LIBAPPOBJ)

appsrc/% : EXTRALIBS=-pthread
appsrc/% : appsrc/%.c lib/libapp.a
	$(CC) $(DEPFLAGS) $(LDFLAGS) $(CFLAGS) $< $(LDLIBS) -o $@

testsrc/%: EXTRALIBS=-lpthread
testsrc/%: testsrc/%.c lib/libapp.a
	$(CC) $(DEPFLAGS) $(LDFLAGS) $(CFLAGS) $< $(LDLIBS) -o $@

clean:
	rm -f $(TEST)
	rm -f $(APP)
	rm -f lib/*.a
	rm -f obj/*.o
#	./clean.sh

