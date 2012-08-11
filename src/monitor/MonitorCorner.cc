//
//	File MonitorCorner.cc
//
//	Monitor Corner module for avoid obstacle
//
//	Framework is originally written by Yunho Jeon
//
//	Written by Insop Song
//	1997 12 1
//	ver 0.1
//

#include "dfl.h"
#include "MonitorCorner.h"

unsigned MonitorCorner::range[16];
unsigned MonitorCorner::detected[8];

MonitorCorner::MonitorCorner(int _priority, int _samplingPeriod, 
		int stackSize, gyro *aG, usonic *aU, wheel *aW)
{

  Error( ErrorDebug, "MonitorDooCorner initializing");
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

  taskId 	= taskSpawn("MonitorCorner", priority, VX_FP_TASK, 
				stackSize, (FUNCPTR) taskModuleStub,
				(int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  Assert(taskId!=ERROR);

  semTake(taskExitSem, WAIT_FOREVER);
  
  tickerTaskId	= taskSpawn("MonitorCorner", TickerTaskPriority, 0, 
				TickerTaskStackSize,
				(FUNCPTR) taskModuleTickerStub,
				(int) this,
				0, 0, 0, 0,
				0, 0, 0, 0, 0);
  Assert(tickerTaskId!=ERROR);
  semTake(tickerExitSem, WAIT_FOREVER);

  corner = 0; // set initial monitor signal ,false
}

MonitorCorner::~MonitorCorner()
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

  Error( ErrorDebug, "MonitorCorner destroyed");
}

//
// C stub functions to call C++ member functions from
// a newly spawned task
//

void MonitorCorner::taskModuleStub(MonitorCorner *This, int d2, int d3, 
					int d4, int d5, int d6, int d7, 
					int d8, int d9, int d10)
{
  Error( ErrorDebug, "MonitorCorner taskModuleStub ");
  This->task();
}

void MonitorCorner::taskModuleTickerStub(MonitorCorner *This, int d2, 
			      int d3, int d4, int d5, int d6, int d7, 
			      int d8, int d9, int d10)
{
  Error( ErrorDebug, "MonitorCorner taskModuleTickerStub");
  This->ticker();
}

void MonitorCorner::ticker(void)
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

void MonitorCorner::task(void)
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
    Error(ErrorDebug,"MonitorCorner");

    //
    // check Corner all side sensors , 
    //
    // configuration  [9]-[10]-[11]-[12]-[13]-[14]-[15] 
    //

    if(corner == 0) {	// Corner start check
      if(range[12]>upTHRESHOLDcorner) {
	detectedNumLower = 0;
	detectedNumUpper = 0;
	for(int i=9;i<12;i++) {
	  if(range[i]<THRESHOLDcorner) {
	    detectedNumLower++;
	  }
	}
	for( i=13;i<16;i++) {
	  if(range[i]<THRESHOLDcorner) {
	    detectedNumUpper++;
	  }
	}

	Error2(ErrorWarning," %d  %d ", detectedNumLower, detectedNumUpper);

	if((detectedNumLower >=2)&&(detectedNumUpper <= 1)) {
	  corner = 1;
	  Error( ErrorDebug, "Corner detected" );
	}

      } 
    } else if(corner ==1){		// Corner end check
      if(range[13]<endCorner) {
	corner = 0;
	Error( ErrorDebug, "Corner End" );
      }
    }
  }
  semGive(taskExitSem); 
  // task exit - deleted automatically
}

int MonitorCorner::isCorner(void)
{
  return corner;
}
  
    
int MonitorCorner::init(void)
{
  corner = 0;	// set initially Corner = false
}

int MonitorCorner::step(void)
{
  taskMode=TaskStep;
  semGive(taskSem);
  return 0;
}

int MonitorCorner::run(void)
{
  taskMode=TaskRunning;
  semGive(tickerSem);
  return 0;
}

int MonitorCorner::stop(void)
{
  taskMode=TaskStopped;
  return 0;
}

