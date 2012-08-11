#
#CC=gcc
CC=/usr/local/m68k-wrs-vxworks/bin/gcc
AR=/usr/local/m68k-wrs-vxworks/bin/ar
RANLIB=/usr/local/m68k-wrs-vxworks/bin/ranlib
CFLAGS=-c -O -fno-builtin -DDEBUG

# Dlm.o DlmMgr.o 
OBJS=DPObj.o DPU.o PortValue.o IPort.o OPort.o TModule.o error.o \
	mvPort.o orIPort.o andIPort.o
LIB=libdfl.a
COMMON_INCLUDES=dfl.h list.h error.h
INCLUDES=$(COMMON_INCLUDES) mvPort.h orIPort.h  andIPort.h

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

mvPort.o: mvPort.cc $(COMMON_INCLUDES) mvPort.h

orIPort.o: orIPort.cc $(COMMON_INCLUDES) orIPort.h

andIPort.o: andIPort.cc $(COMMON_INCLUDES) andIPort.h
