#
#CC=gcc
CC=/usr/local/m68k-wrs-vxworks/bin/gcc
AR=/usr/local/m68k-wrs-vxworks/bin/ar
RANLIB=/usr/local/m68k-wrs-vxworks/bin/ranlib
CFLAGS=-c -O -fno-builtin -DDEBUG

# Dlm.o DlmMgr.o 
OBJS=error.o 
COMMON_INCLUDES=list.h error.h
INCLUDES=$(COMMON_INCLUDES) 

all: $(OBJS)

install: all
	cp $(OBJS) ../lib
	cp $(INCLUDES) ../include

.cc.o:
	$(CC) $(CFLAGS) $*.cc

$(OBJS): $(COMMON_INCLUDES)

