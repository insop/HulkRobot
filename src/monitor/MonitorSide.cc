//
//	File MonitorSide.cc
//
//	Monitor Side module for avoid obstacle
//
//	Framework is originally written by Yunho Jeon
//
//	Written by Insop Song
//	1997 12 1
//	ver 0.1
//

#include "dfl.h"
#include "MonitorSide.h"

unsigned MonitorSide::range[16];

MonitorSide::MonitorSide(int _priority, int _samplingPeriod, 
		int stackSize, gyro *aG, usonic *aU, wheel *aW)
{

  Error( ErrorDebug, "MonitorSide initializing");
  priority 	= _priority;
  samplingPeriod= _samplingPeriod;
  delayTicks	= samplingPeriod/OSTickPeriod;

  taskMode	= TaskStopped;
  taskSem	= semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
  tickerSem	= semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
  taskExitSem	= semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
  tickerExitSem	= semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);

  aGyro = aG;
  aUSonic = aU;
  aWheel = aW;

  Assert( taskSem && taskExitSem && tickerSem && tickerExitSem );

  taskId 	= taskSpawn("MonitorSide", priority, VX_FP_TASK, 
				stackSize, (FUNCPTR) taskModuleStub,
				(int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  Assert(taskId!=ERROR);

  semTake(taskExitSem, WAIT_FOREVER);
  
  tickerTaskId	= taskSpawn("MonitorSide", TickerTaskPriority, 0, 
				TickerTaskStackSize,
				(FUNCPTR) taskModuleTickerStub,
				(int) this,
				0, 0, 0, 0,
				0, 0, 0, 0, 0);
  Assert(tickerTaskId!=ERROR);
  semTake(tickerExitSem, WAIT_FOREVER);

  side = 0; // set initial monitor signal ,false
}

MonitorSide::~MonitorSide()
{
  taskMode=TaskTerminate;
  semGive(tickerSem);
  semGive(taskSem);
  semTake(tickerExitSem, WAIT_FOREVER);
  semTake(taskExitSem, WAIT_FOREVER);
  semDelete(taskSem);
  semDelete(taskExitSem);
  semDelete(tickerSem);
  semDelete(tickerExitSem);

  Error( ErrorDebug, "MonitorSide destroyed");
}

//
// C stub functions to call C++ member functions from
// a newly spawned task
//

void MonitorSide::taskModuleStub(MonitorSide *This, int d2, int d3, 
					int d4, int d5, int d6, int d7, 
					int d8, int d9, int d10)
{
  Error( ErrorDebug, "MonitorSide taskModuleStub ");
  This->task();
}

void MonitorSide::taskModuleTickerStub(MonitorSide *This, int d2, 
			      int d3, int d4, int d5, int d6, int d7, 
			      int d8, int d9, int d10)
{
  Error( ErrorDebug, "MonitorSide taskModuleTickerStub");
  This->ticker();
}

void MonitorSide::ticker(void)
{
  semGive(tickerExitSem);
  while(taskMode!=TaskTerminate) {

    if(taskMode==TaskStopped || taskMode==TaskPaused) 
       semTake(tickerSem, WAIT_FOREVER);

    while(taskMode==TaskRunning) {
      Error( ErrorNone, "ticker::semGive" );
      semGive(taskSem);
      taskDelay(delayTicks);
    }
  }
  semGive(tickerExitSem);
  // task exit - deleted automatically
}

void MonitorSide::task(void)
{
  semGive(taskExitSem); 		// signal that it has been created
  while(taskMode!=TaskTerminate) {
    semTake(taskSem, WAIT_FOREVER);	// wait until ticker ticks
    if (taskMode!=TaskRunning && taskMode!=TaskStep) continue;
    Error( ErrorNone, "task tick" );

    // task code HERE
    for(int i=0;i<16;i++) {
      range[i] = aUSonic->usData(i);
    }
    Error(ErrorDebug,"MonitorSide");
    //printf("[13] %d [12] %d [11] %d \n",range[13],range[12],range[11]);

    //
    // check Side 5 sensors , configuration  [13]-[12]-[11]
    //

    if((range[12]>SideZone) && (range[12]<SideMaxZone)) {
      side = 1;
      Error( ErrorDebug, "Side is too far" );
    }
    else {
      side = 0;
    }
  }
  semGive(taskExitSem); 
  // task exit - deleted automatically
}

int MonitorSide::isSide(void)
{
  return side;
}
  
    
int MonitorSide::init(void)
{
}

int MonitorSide::step(void)
{
  taskMode=TaskStep;
  semGive(taskSem);
  return 0;
}

int MonitorSide::run(void)
{
  taskMode=TaskRunning;
  semGive(tickerSem);
  return 0;
}

int MonitorSide::stop(void)
{
  taskMode=TaskStopped;
  return 0;
}

