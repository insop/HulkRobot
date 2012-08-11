//
// File dfl.h
//
// Data Flow Layer
//
// Written by Yunho Jeon
// Division of Electronics and Information
// KIST
//

//
// This file contains class definitions and constants values for
// data flow layer of human robot control architecture
//

#ifndef _DPL_H_
#define _DPL_H_
#include <vxWorks.h>
#include <semLib.h>
#include <taskLib.h>
#include <string.h>
#include "list.h"
#include "error.h"


//
// Common Data Types
//

typedef unsigned char byte;
typedef signed char sByte;


//
// Constants
//

const int MaxObjName		= 32;
const int MaxTaskName		= 32;
const int MaxDescription	= 256;
const int TickerTaskPriority	= 10;
const int TickerTaskStackSize	= 8192;
const int OSTickPeriod		= 1;		// 1000 Hz

enum {			// Basic data types
  DataTypeSByte,
  DataTypeSByteArray,
  DataTypeSByte2DArray,
  DataTypeInt,
  DataTypeIntArray,
  DataTypeInt2DArray,
  DataTypeFloat,
  DataTypeFloatArray, 
  DataTypeFloat2DArray
};



#endif // _DPL_H_
