#include "dfl.h"
#include "AlignWall.h"

unsigned AlignWall::forward;
unsigned AlignWall::middle;
unsigned AlignWall::reward;
unsigned AlignWall::difference;

AlignWall::AlignWall(int _priority, int _samplingPeriod, 
		int stackSize, gyro *aG, usonic *aU, wheel *aW)
{

  Error( ErrorDebug, "AlignWall initializing");
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

  taskId 	= taskSpawn("AlignWall", priority, VX_FP_TASK, 
				stackSize, (FUNCPTR) taskModuleStub,
				(int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  Assert(taskId!=ERROR);

  semTake(taskExitSem, WAIT_FOREVER);
  
  tickerTaskId	= taskSpawn("AlignWall", TickerTaskPriority, 0, 
				TickerTaskStackSize,
				(FUNCPTR) taskModuleTickerStub,
				(int) this,
				0, 0, 0, 0,
				0, 0, 0, 0, 0);
  Assert(tickerTaskId!=ERROR);
  semTake(tickerExitSem, WAIT_FOREVER);
}

AlignWall::~AlignWall()
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

  Error( ErrorDebug, "AlignWall destroyed");
}

//
// C stub functions to call C++ member functions from
// a newly spawned task
//

void AlignWall::taskModuleStub(AlignWall *This, int d2, int d3, 
					int d4, int d5, int d6, int d7, 
					int d8, int d9, int d10)
{
  Error( ErrorDebug, "AlignWall taskModuleStub ");
  This->task();
}

void AlignWall::taskModuleTickerStub(AlignWall *This, int d2, 
			      int d3, int d4, int d5, int d6, int d7, 
			      int d8, int d9, int d10)
{
  Error( ErrorDebug, "AlignWall taskModuleTickerStub");
  This->ticker();
}

void AlignWall::ticker(void)
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

void AlignWall::task(void)
{
  semGive(taskExitSem); 		// signal that it has been created
  while(taskMode!=TaskTerminate) {
    semTake(taskSem, WAIT_FOREVER);	// wait until ticker ticks
    if (taskMode!=TaskRunning && taskMode!=TaskStep) continue;
    Error( ErrorNone, "task tick" );

    // task code HERE
    forward = aUSonic->usData(15);
    middle =  aUSonic->usData(14);
    reward =  aUSonic->usData(13);
    difference = abs(forward - reward);
    //printf(" [F] %d  [M], %d  [R],%d\n",forward,middle, reward);

    //
    // check side 3 sensors , configuration  [15]-[14]-[13]
    //

    //
    // basic rule
    //

    if(forward > AlignDistance) {
      turnMode = TURN_LEFT;
      Error( ErrorNone, "TURN_LEFT for alignning" );
    } 
    else {
      turnMode = TURN_STOP;
      Error( ErrorNone, "TURN_STOP for alignning" );
    }

/*
    // additional rule
    //

    if(middle < sIdeRange) {
      turnMode = TURN_RIGHT;
      Error( ErrorDebug, "TURN_RIGHT for minimize side" );
    }

    //
    // for help MonitorDoor and MonitorCorner
    //

    if((forward>middle)&&(forward-middle)>100) {
      turnMode = TURN_STOP_SLOW;
      Error( ErrorDebug, "TURN_STOP for help MonitorDoor" );
    }

*/

    switch(turnMode) {
      case(TURN_LEFT):
	aWheel->setSpeed(0.1);
	aWheel->setTurnSpeed(-0.05);
	break;
      case(TURN_STOP):
	aWheel->setSpeed(0.1);
	aWheel->setTurnSpeed(0.0);
	break;
    }
    // task code till HERE

  }
    // for stop when finish this task
	aWheel->setSpeed(0.0);
	aWheel->setTurnSpeed(0.0);

  semGive(taskExitSem); 
  // task exit - deleted automatically
}

int AlignWall::init(void)
{
}

int AlignWall::step(void)
{
  taskMode=TaskStep;
  semGive(taskSem);
  return 0;
}

int AlignWall::run(void)
{
  taskMode=TaskRunning;
  semGive(tickerSem);
  return 0;
}

int AlignWall::stop(void)
{
  taskMode=TaskStopped;
  return 0;
}

