#include "dfl.h"
#include "FollowWall.h"

unsigned FollowWall::forward;
unsigned FollowWall::middle;
unsigned FollowWall::reward;
unsigned FollowWall::difference;

FollowWall::FollowWall(int _priority, int _samplingPeriod, 
		int stackSize, gyro *aG, usonic *aU, wheel *aW)
{

  Error( ErrorDebug, "FollowWall initializing");
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

  taskId 	= taskSpawn("FollowWall", priority, VX_FP_TASK, 
				stackSize, (FUNCPTR) taskModuleStub,
				(int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  Assert(taskId!=ERROR);

  semTake(taskExitSem, WAIT_FOREVER);
  
  tickerTaskId	= taskSpawn("FollowWall", TickerTaskPriority, 0, 
				TickerTaskStackSize,
				(FUNCPTR) taskModuleTickerStub,
				(int) this,
				0, 0, 0, 0,
				0, 0, 0, 0, 0);
  Assert(tickerTaskId!=ERROR);
  semTake(tickerExitSem, WAIT_FOREVER);
}

FollowWall::~FollowWall()
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

  Error( ErrorDebug, "FollowWall destroyed");
}

//
// C stub functions to call C++ member functions from
// a newly spawned task
//

void FollowWall::taskModuleStub(FollowWall *This, int d2, int d3, 
					int d4, int d5, int d6, int d7, 
					int d8, int d9, int d10)
{
  Error( ErrorDebug, "FollowWall taskModuleStub ");
  This->task();
}

void FollowWall::taskModuleTickerStub(FollowWall *This, int d2, 
			      int d3, int d4, int d5, int d6, int d7, 
			      int d8, int d9, int d10)
{
  Error( ErrorDebug, "FollowWall taskModuleTickerStub");
  This->ticker();
}

void FollowWall::ticker(void)
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

void FollowWall::task(void)
{
  semGive(taskExitSem); 		// signal that it has been created
  while(taskMode!=TaskTerminate) {
    semTake(taskSem, WAIT_FOREVER);	// wait until ticker ticks
    if (taskMode!=TaskRunning && taskMode!=TaskStep) continue;
    Error( ErrorNone, "task tick" );

    // task code HERE
    forward = aUSonic->usData(13);
    middle =  aUSonic->usData(12);
    reward =  aUSonic->usData(11);
    difference = abs(forward - reward);
    //printf(" [F] %d  [M], %d  [R],%d\n",forward,middle, reward);

    //
    // check side 3 sensors , configuration  [13]-[12]-[11]
    //

    //
    // basic rule
    //

    if(forward < reward) {
      turnMode = TURN_RIGHT;
      Error( ErrorNone, "TURN_RIGHT" );
    } 
    else if(forward>reward) {
      turnMode = TURN_LEFT;
      Error( ErrorNone, "TURN_LEFT" );
    }

    //
    // additional rule
    //

/*
    if(middle < SideRange) {
      turnMode = TURN_RIGHT;
      Error( ErrorNone, "TURN_RIGHT for minimize side" );
    }
*/

    //
    // for help MonitorDoor and MonitorCorner
    //

    if((forward>middle)&&(forward-middle)>100) {
      turnMode = TURN_STOP_SLOW;
      Error( ErrorNone, "TURN_STOP for help MonitorDoor" );
    }


    switch(turnMode) {
      case(TURN_RIGHT):
	aWheel->setSpeed(0.1);
	aWheel->setTurnSpeed(0.02);
	break;
      case(TURN_LEFT):
	aWheel->setSpeed(0.1);
	aWheel->setTurnSpeed(-0.02);
	break;
      case(TURN_STOP):
	aWheel->setSpeed(0.1);
	aWheel->setTurnSpeed(0.0);
	break;
      case(TURN_STOP_SLOW):
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

int FollowWall::init(void)
{
}

int FollowWall::step(void)
{
  taskMode=TaskStep;
  semGive(taskSem);
  return 0;
}

int FollowWall::run(void)
{
  taskMode=TaskRunning;
  semGive(tickerSem);
  return 0;
}

int FollowWall::stop(void)
{
  taskMode=TaskStopped;
  return 0;
}

