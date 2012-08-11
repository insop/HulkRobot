#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <taskLib.h>
#include "error.h"
#include "list.h"
#include "tyLib.h"
#include "stdio.h"
#include "ioLib.h"

const int MaxTask=100;
const int MaxExitFunc=100;

static int taskList[MaxTask];
static void (*exitFuncList[MaxExitFunc])(void);
static int numTask=0;
static int numExitFunc=0;
static FILE *serialDev;

int errorThreshold;

void initError(int _errorThreshold)
{
  printf( "Error initialized\n" );
  errorThreshold=_errorThreshold;
  numTask=0;
  numExitFunc=0;
  
  //  for serial monitor
/*
  serialDev=fopen("/tyCo/3", "r+");
  if (serialDev==(FILE *)NULL) {
    Error(ErrorFatal, "Unable to open for serial monitor port");
    return;
  }

  if(ioctl(fileno(serialDev), FIOBAUDRATE, 19200)==ERROR) {
    Error(ErrorFatal, "Unable to set baud rate");
    return;
  }
  */
}

void fatalExit(void)
{
  int i;
  char taskNameStr[32];

  if (taskName(taskIdSelf()))
    strncpy(taskNameStr, taskName(taskIdSelf()), 32);
  else
    strcpy(taskNameStr, "No Name");

  printf( "Fatal Exit Sequence Initiated in task %d(%s)\n", 
		taskIdSelf(),
		taskNameStr );
  printf( "Number of currently registered task is %d\n", numTask );
  //
  // delete all tasks registered except this one
  //

  for(i=0;i<numTask;i++) {
    if (taskList[i]!=taskIdSelf() && taskIdVerify(taskList[i])==OK) {
      if (taskName(taskList[i]))
	strncpy(taskNameStr, taskName(taskList[i]), 32);
      else
	strcpy(taskNameStr, "No Name");

      if (taskDelete(taskList[i])==OK) {
	printf( "Task %d(%s) deleted successfully\n",
		taskList[i],
		taskNameStr );
      } else {
	printf( "Task %d(%s) delete failed\n",
		taskList[i],
		taskNameStr );
      }
    }
  }

  for(i=0;i<numExitFunc;i++)
    (*(exitFuncList[i]))();

  printf( "Fatal Exit Sequence Completed\n" );
  exit(-1);
}
  

void atFatalExit(void (*exitFunc)(void))
{
  int i;
  // register task
  for(i=0;i<numTask;i++)
    if (taskList[i]==taskIdSelf()) break;
  if (i==numTask) {
    if (numTask==MaxTask) {
      Error( ErrorExit, "Maximum number of registered task exceeded" );
    } else {
      taskList[numTask++]=taskIdSelf();
      Error1( ErrorDebug, "Registering task %s", taskName(taskIdSelf()));
    }
  }
  if (!exitFunc) return;
  for(i=0;i<numExitFunc;i++)
    if (exitFuncList[i]==exitFunc) break;
  if (i==numExitFunc) {
    if (numExitFunc==MaxExitFunc) {
	Error( ErrorExit, "Maximum number of exit function exceeded" );
    } else {
      exitFuncList[numExitFunc++]=exitFunc;
    }
  }
}
