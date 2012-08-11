//
//	File AvoidObstacle.cc
//
//	Avoid obstacle Behavior Controller Module
//
//	Framework is originally written by Yunho Jeon
//
//	Written by Insop Song
//	1997 12 1
//	ver 0.1
//

#include "dfl.h"
#include "AvoidObstacle.h"

unsigned AvoidObstacle::range[16];

AvoidObstacle::AvoidObstacle(int _priority, int _samplingPeriod, 
		int stackSize, gyro *aG, usonic *aU, wheel *aW)
{

  Error( ErrorDebug, "AvoidObstacle initializing");
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

  taskId 	= taskSpawn("AvoidObstacle", priority, VX_FP_TASK, 
				stackSize, (FUNCPTR) taskModuleStub,
				(int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  Assert(taskId!=ERROR);

  semTake(taskExitSem, WAIT_FOREVER);
  
  tickerTaskId	= taskSpawn("AvoidObstacle", TickerTaskPriority, 0, 
				TickerTaskStackSize,
				(FUNCPTR) taskModuleTickerStub,
				(int) this,
				0, 0, 0, 0,
				0, 0, 0, 0, 0);
  Assert(tickerTaskId!=ERROR);
  semTake(tickerExitSem, WAIT_FOREVER);
}

AvoidObstacle::~AvoidObstacle()
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

  Error( ErrorDebug, "AvoidObstacle destroyed");
}

//
// C stub functions to call C++ member functions from
// a newly spawned task
//

void AvoidObstacle::taskModuleStub(AvoidObstacle *This, int d2, int d3, 
					int d4, int d5, int d6, int d7, 
					int d8, int d9, int d10)
{
  Error( ErrorDebug, "AvoidObstacle taskModuleStub ");
  This->task();
}

void AvoidObstacle::taskModuleTickerStub(AvoidObstacle *This, int d2, 
			      int d3, int d4, int d5, int d6, int d7, 
			      int d8, int d9, int d10)
{
  Error( ErrorDebug, "AvoidObstacle taskModuleTickerStub");
  This->ticker();
}

void AvoidObstacle::ticker(void)
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

void AvoidObstacle::task(void)
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
    //printf(" [15] %d  [0], %d  [1],%d\n",range[15],range[0],range[1]);

    //
    // check front 3 sensors , configuration  [15]-[0]-[1]
    //

    if((range[15] > DangerZone) && (range[0] > DangerZone)
			      && (range[1] > DangerZone)){
      turnMode = GO;
      Error( ErrorDebug, "Go" );
    } 
    /*
    else if(!(range[15]>DangerZone) && (range[0]>DangerZone) 
      && (range[1]>DangerZone)){
      turnMode = TURN_SOFT_RIGHT;
      Error( ErrorNone, "TURN_SOFT_LEFT" );
    }
    else if((range[15]>DangerZone) && (range[0]>DangerZone) 
      && !(range[1]>DangerZone)){
      turnMode = TURN_SOFT_RIGHT;
      Error( ErrorNone, "TURN_SOFT_RIGHT" );
    }
    */
    else if((!(range[15]>DangerZone) || !(range[0]>DangerZone)) 						/* && (range[1]>DangerZone)*/ ){
      turnMode = TURN_HARD_RIGHT;
      Error( ErrorNone, "TURN_HARD_RIGHT" );
    }
    else if(/* (range[15]>DangerZone) && */ (!(range[0]>DangerZone) 
				 || !(range[1]>DangerZone)) ){
      turnMode = TURN_HARD_LEFT;
      Error( ErrorNone, "TURN_HARD_LEFT" );
    }


/*
 * for stop snd back condition 
 *
    if((!(range[15] > StopZone) || !(range[0] > StopZone)
         || !(range[1] > StopZone)) && !(turnMode == STOP)	
				     &&	!(turnMode == BACK)){
      turnMode = STOP;
      Error( ErrorNone, "STOP" );
    } 
    if((!(range[15] > DangerZone) || !(range[0] > DangerZone)
	      || !(range[1] > DangerZone)) && (turnMode==STOP)){
      turnMode = BACK;
      Error( ErrorNone, "BACK" );
    } 

 *
 *
 */



    switch(turnMode) {
      case(GO):
	aWheel->setSpeed(0.1);
	aWheel->setTurnSpeed(0.0);
	break;
      case(TURN_SOFT_RIGHT):
	aWheel->setSpeed(0.05);
	aWheel->setTurnSpeed(0.05);
	break;
      case(TURN_SOFT_LEFT):
	aWheel->setSpeed(0.05);
	aWheel->setTurnSpeed(-0.05);
	break;
      case(TURN_HARD_RIGHT):
	aWheel->setSpeed(0.01);
	aWheel->setTurnSpeed(0.05);
	break;
      case(TURN_HARD_LEFT):
	aWheel->setSpeed(0.01);
	aWheel->setTurnSpeed(-0.05);
	break;
      case(STOP):
	aWheel->setSpeed(0.0);
	aWheel->setTurnSpeed(0.0);
	break;
      case(BACK):
	aWheel->setSpeed(-0.05);
	aWheel->setTurnSpeed(0.0);
	break;
    }
    // task code till HERE

  }
	aWheel->setSpeed(0.0);
	aWheel->setTurnSpeed(0.0);

  semGive(taskExitSem); 
  // task exit - deleted automatically
}

unsigned AvoidObstacle::minimum(unsigned a,unsigned b,unsigned c)
{
  unsigned min = a;
  if(min>b)
    min = b;
  if(min>c);
    min = c;

  return c;
}
  
    
int AvoidObstacle::init(void)
{
}

int AvoidObstacle::step(void)
{
  taskMode=TaskStep;
  semGive(taskSem);
  return 0;
}

int AvoidObstacle::run(void)
{
  taskMode=TaskRunning;
  semGive(tickerSem);
  return 0;
}

int AvoidObstacle::stop(void)
{
  taskMode=TaskStopped;
  return 0;
}

