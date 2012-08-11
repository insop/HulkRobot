#
#CC=gcc
CC=/usr/local/m68k-wrs-vxworks/bin/gcc
AR=/usr/local/m68k-wrs-vxworks/bin/ar
RANLIB=/usr/local/m68k-wrs-vxworks/bin/ranlib
CFLAGS=-c -O -fno-builtin -I../include -DDEBUG

OBJS=MonitorFront.o MonitorSide.o MonitorDoor.o MonitorCorner.o

LIB=libMon.a
COMMON_INCLUDES=MonitorFront.h MonitorSide.h MonitorDoor.h MonitorCorner.h
INCLUDES=$(COMMON_INCLUDES) 

all: $(LIB)

install: all
	cp $(LIB) ../lib
	cp $(INCLUDES) ../include

$(LIB): $(OBJS)
	$(AR) ru $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

.cc.o:
	$(CC) $(CFLAGS) $*.cc

$(OBJS): $(COMMON_INCLUDES)
