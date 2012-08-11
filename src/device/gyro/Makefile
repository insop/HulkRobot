#
CC=/usr/local/m68k-wrs-vxworks/bin/gcc
AR=/usr/local/m68k-wrs-vxworks/bin/ar
LD=/usr/local/m68k-wrs-vxworks/bin/ld
LIBS=../../lib/libdfl.a /usr/local/lib/gcc-lib/m68k-wrs-vxworks/2.6.2/libgcc.a
CFLAGS=-c -O -fno-builtin -I../../include -DDEBUG

OBJS=gyro.o 
INCLUDES=gyro.h 

all: $(OBJS) 

install: all
	cp $(INCLUDES) ../../include
	cp $(OBJS) ../../lib

$(OBJS): $(INCLUDES)

.cc.o:
	$(CC) $(CFLAGS) $*.cc
