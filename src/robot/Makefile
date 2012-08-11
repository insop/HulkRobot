#
CC=/usr/local/m68k-wrs-vxworks/bin/gcc
AR=/usr/local/m68k-wrs-vxworks/bin/ar
LD=/usr/local/m68k-wrs-vxworks/bin/ld
LIBS=../../lib/libdfl.a /usr/local/lib/gcc-lib/m68k-wrs-vxworks/2.6.2/libgcc.a
CFLAGS=-c -O -fno-builtin -I../include -DDEBUG

MODULE=robot.mod
OBJS=robot.o
INCLUDES=robot.h ../inc../speed.h ../../include/gyro.h

all: $(OBJS) 

install: all
	cp robot.h ../include
	cp robot.o ../lib

$(MODULE): $(OBJS)
	$(LD) -r -o $(MODULE) $(OBJS) $(LIBS)

$(OBJS): $(INCLUDES)

.cc.o:
	$(CC) $(CFLAGS) $*.cc
