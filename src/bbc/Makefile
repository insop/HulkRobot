#
#CC=gcc
CC=/usr/local/m68k-wrs-vxworks/bin/gcc
AR=/usr/local/m68k-wrs-vxworks/bin/ar
RANLIB=/usr/local/m68k-wrs-vxworks/bin/ranlib
CFLAGS=-c -O -fno-builtin -I../include -DDEBUG

OBJS=AvoidObstacle.o FollowWall.o AlignWall.o GoStraight.o TurnCorner.o

LIB=libBBC.a
COMMON_INCLUDES=AvoidObstacle.h FollowWall.h AlignWall.h GoStraight.h TurnCorner.h
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
