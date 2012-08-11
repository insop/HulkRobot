//
//	File MonitorFront.cc
//
//	Monitor front module for avoid obstacle
//
//	Framework is originally written by Yunho Jeon
//
//	Written by Insop Song
//	1997 12 1
//	ver 0.1
//

#include "dfl.h"
#include "MonitorFront.h"

unsigned MonitorFront::range[16];

MonitorFront::MonitorFront(int _priority, int _samplingPeriod, 
		int stackSize, gyro *aG, usonic *aU, wheel *aW)
{

  Error( ErrorDebug, "MonitorFront initializing");
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

  taskId 	= taskSpawn("MonitorFront", priority, VX_FP_TASK, 
				stackSize, (FUNCPTR) taskModuleStub,
				(int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  Assert(taskId!=ERROR);

  semTake(taskExitSem, WAIT_FOREVER);
  
  tickerTaskId	= taskSpawn("MonitorFront", TickerTaskPriority, 0, 
				TickerTaskStackSize,
				(FUNCPTR) taskModuleTickerStub,
				(int) this,
				0, 0, 0, 0,
				0, 0, 0, 0, 0);
  Assert(tickerTaskId!=ERROR);
  semTake(tickerExitSem, WAIT_FOREVER);

  front = 0; // set initial monitor signal ,false
}

MonitorFront::~MonitorFront()
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

  Error( ErrorDebug, "MonitorFront destroyed");
}

//
// C stub functions to call C++ member functions from
// a newly spawned task
//

void MonitorFront::taskModuleStub(MonitorFront *This, int d2, int d3, 
					int d4, int d5, int d6, int d7, 
					int d8, int d9, int d10)
{
  Error( ErrorDebug, "MonitorFront taskModuleStub ");
  This->task();
}

void MonitorFront::taskModuleTickerStub(MonitorFront *This, int d2, 
			      int d3, int d4, int d5, int d6, int d7, 
			      int d8, int d9, int d10)
{
  Error( ErrorDebug, "MonitorFront taskModuleTickerStub");
  This->ticker();
}

void MonitorFront::ticker(void)
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

void MonitorFront::task(void)
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
    Error(ErrorDebug,"MonitorFront");
    //printf("[14] %d [15] %d [0] %d [1] %d [2] %d\n",range[14],range[15],range[0],range[1], range[2]);

    //
    // check front 5 sensors , configuration  [14]-[15]-[0]-[1]-[2]
    //

    if(!(range[14]>FrontZone) || !(range[15]>FrontZone) ||
       !(range[0]>FrontZone) || !(range[1]>FrontZone) ||
			        ! (range[2]>FrontZone)){
      front = 1;
      Error( ErrorDebug, "Front Obstacle detected" );
    }
    else {
      front = 0;
    }
  }
  semGive(taskExitSem); 
  // task exit - deleted automatically
}

int MonitorFront::isFront(void)
{
  return front;
}
  
    
int MonitorFront::init(void)
{
}

int MonitorFront::step(void)
{
  taskMode=TaskStep;
  semGive(taskSem);
  return 0;
}

int MonitorFront::run(void)
{
  taskMode=TaskRunning;
  semGive(tickerSem);
  return 0;
}

int MonitorFront::stop(void)
{
  taskMode=TaskStopped;
  return 0;
}

