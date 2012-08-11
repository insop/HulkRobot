//
//	File MonitorDoor.cc
//
//	Monitor Door module for avoid obstacle
//
//	Framework is originally written by Yunho Jeon
//
//	Written by Insop Song
//	1997 12 1
//	ver 0.1
//

#include "dfl.h"
#include "MonitorDoor.h"

unsigned MonitorDoor::range[16];
unsigned MonitorDoor::detected[8];

MonitorDoor::MonitorDoor(int _priority, int _samplingPeriod, 
		int stackSize, gyro *aG, usonic *aU, wheel *aW)
{

  Error( ErrorDebug, "MonitorDooDoor initializing");
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

  taskId 	= taskSpawn("MonitorDoor", priority, VX_FP_TASK, 
				stackSize, (FUNCPTR) taskModuleStub,
				(int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  Assert(taskId!=ERROR);

  semTake(taskExitSem, WAIT_FOREVER);
  
  tickerTaskId	= taskSpawn("MonitorDoor", TickerTaskPriority, 0, 
				TickerTaskStackSize,
				(FUNCPTR) taskModuleTickerStub,
				(int) this,
				0, 0, 0, 0,
				0, 0, 0, 0, 0);
  Assert(tickerTaskId!=ERROR);
  semTake(tickerExitSem, WAIT_FOREVER);

  door = 0; // set initial monitor signal ,false
}

MonitorDoor::~MonitorDoor()
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

  Error( ErrorDebug, "MonitorDoor destroyed");
}

//
// C stub functions to call C++ member functions from
// a newly spawned task
//

void MonitorDoor::taskModuleStub(MonitorDoor *This, int d2, int d3, 
					int d4, int d5, int d6, int d7, 
					int d8, int d9, int d10)
{
  Error( ErrorDebug, "MonitorDoor taskModuleStub ");
  This->task();
}

void MonitorDoor::taskModuleTickerStub(MonitorDoor *This, int d2, 
			      int d3, int d4, int d5, int d6, int d7, 
			      int d8, int d9, int d10)
{
  Error( ErrorDebug, "MonitorDoor taskModuleTickerStub");
  This->ticker();
}

void MonitorDoor::ticker(void)
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

void MonitorDoor::task(void)
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
    Error(ErrorDebug,"MonitorDoor");

    //
    // check Door all side sensors , 
    //
    // configuration  [9]-[10]-[11]-[12]-[13]-[14]-[15] 
    //

    if(door == 0) {	// door start check
      if(range[12]>upTHRESHOLD) {
	detectedNumLower = 0;
	detectedNumUpper = 0;
	for(int i=9;i<12;i++) {
	  if(range[i]<THRESHOLD) {
	    detectedNumLower++;
	  }
	}
	for( i=13;i<16;i++) {
	  if(range[i]<THRESHOLD) {
	    detectedNumUpper++;
	  }
	}

	if((detectedNumLower >=2)&&(detectedNumUpper >=1)) {
	  door = 1;
	  Error( ErrorDebug, "Door detected" );
	}

      } 
    } else if(door ==1){		// door end check
      if(range[12]<endDOOR) {
	door = 0;
	Error( ErrorDebug, "Door End" );
      }
    }
  }
  semGive(taskExitSem); 
  // task exit - deleted automatically
}

int MonitorDoor::isDoor(void)
{
  return door;
}
  
    
int MonitorDoor::init(void)
{
  door = 0;	// set initially door = false
}

int MonitorDoor::step(void)
{
  taskMode=TaskStep;
  semGive(taskSem);
  return 0;
}

int MonitorDoor::run(void)
{
  taskMode=TaskRunning;
  semGive(tickerSem);
  return 0;
}

int MonitorDoor::stop(void)
{
  taskMode=TaskStopped;
  return 0;
}

