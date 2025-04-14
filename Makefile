# Variables
CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC
AR = ar
ARFLAGS = rcs
PREFIX = /usr/local
LIBNAME = libpicohttpparser

# Source files
SRCS = picohttpparser.c
OBJS = $(SRCS:.c=.o)

# Targets
all: lib$(LIBNAME).so lib$(LIBNAME).a

lib$(LIBNAME).so: $(OBJS)
	$(CC) -shared -o $@ $^

lib$(LIBNAME).a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

install: all
	install -d $(PREFIX)/lib
	install -m 755 lib$(LIBNAME).so $(PREFIX)/lib
	install -m 644 lib$(LIBNAME).a $(PREFIX)/lib
	install -d $(PREFIX)/include
	install -m 644 picohttpparser.h $(PREFIX)/include

uninstall:
	rm -f $(PREFIX)/lib/lib$(LIBNAME).so
	rm -f $(PREFIX)/lib/lib$(LIBNAME).a
	rm -f $(PREFIX)/include/picohttpparser.h

clean:
	rm -f $(OBJS) lib$(LIBNAME).so lib$(LIBNAME).a

.PHONY: all install uninstall clean
