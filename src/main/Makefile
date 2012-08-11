#
CC=/usr/local/m68k-wrs-vxworks/bin/gcc
AR=/usr/local/m68k-wrs-vxworks/bin/ar
LD=/usr/local/m68k-wrs-vxworks/bin/ld
LIBS=/usr/local/lib/gcc-lib/m68k-wrs-vxworks/2.6.2/libgcc.a
CFLAGS=-c -O -fno-builtin -I../include -DDEBUG

MODULE=hulk.mod
OBJS=hulk.o
INCLUDES=
EXTRALIBS=../lib/error.o ../lib/wheel.o ../lib/usonic.o ../lib/gyro.o ../lib/libBBC.a ../lib/libMon.a

all: $(MODULE) 

install: all
	cp $(MODULE) /home1/usr/cessna/hulk

$(MODULE): $(OBJS) $(LIBS) $(EXTRALIBS)
	$(LD) -r $(LFLAGS) -o $(MODULE)  $(OBJS) $(EXTRALIBS) $(LIBS)

$(OBJS): $(INCLUDES)

.cc.o:
	$(CC) $(CFLAGS)  $*.cc
