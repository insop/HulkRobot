//
// wheel.cc
//
// This module controls TMC2A TurboTek motor controller board
//
// Orignally written by Yunho Jeon
// Human Robot Project
// KIST
//
// Modified by Insop Song
//
// 1997 12 1
// ver 0.5
//

#include "dfl.h"
#include "wheel.h"

#define _MODULE_NAME_ wheel 
#define REAL_HARDWARE	// comment out this line to run without hardware
#include "wheel.h"
extern "C" {
#include "speed.h"
}

SEM_ID wheel::scanTaskSem;
int wheel::scanTaskId;
int wheel::scanTaskQuit;
int wheel::delayTicks;
int wheel::numOpen;
float wheel::targetSpeed;
float wheel::targetTurn;
gyro *wheel::aGyro;


float wheel::maxSpeedDiff;
float wheel::maxTurnDiff;
float wheel::lastSpeed;
float wheel::lastTurn;
// 
// hardware related routines
//

void dspInit(void)
{
#ifdef REAL_HARDWARE
#endif
}

//
// Wheel Class Member Functions
//

wheel::wheel(int samplingPeriod, gyro *aG)
{
  aGyro = aG;
  dspInit();
  speed_command(0,0);
  speed_command(1,0);

  maxSpeedDiff 		= MaxAcc*(float)samplingPeriod/1000.0;
  maxTurnDiff		= MaxRAcc*(float)samplingPeriod/1000.0;
  Error2(ErrorDebug, "maxSpeedDiff=%f, maxTurnDiff=%f",
			maxSpeedDiff, maxTurnDiff);
  lastSpeed		= 0.0;
  lastTurn		= 0.0;
  targetSpeed   = 0;
  targetTurn 	= 0;

  delayTicks	= samplingPeriod/OSTickPeriod;

  if(numOpen==0) {
    createScanTask();
  }
  numOpen++;
  Error( ErrorDebug, "Wheel module initializing");
}

void wheel::createScanTask()
{

  scanTaskSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
  scanTaskQuit=0;
  scanTaskId 	= taskSpawn("wheelScanTask", 10,VX_FP_TASK, 64*1024,
				(FUNCPTR) scanTask,
				0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0);
}		

void wheel::terminateScanTask()
{
  scanTaskQuit=1;
  // wait until scan task quits 
  if(semTake(scanTaskSem, OSTickPeriod*1000)==ERROR) {	// wait 1 sec
    // kill the task
    Error(ErrorWarning, "Terminating scan task failed");
    Error(ErrorWarning, "maybe wheel system is not responding?");
    if(taskDelete(scanTaskId)==ERROR) {
      Error(ErrorFatal, "Deleting scan task failed");
    }
  }
  semDelete(scanTaskSem);
  Error(ErrorDebug, "Scan task terminated" );
}


wheel::~wheel()
{
  numOpen--;
  if(numOpen==0)
    terminateScanTask();
}

int wheel::setSpeed(float _targetSpeed)
{
  targetSpeed = _targetSpeed;
}

// turn speed
int wheel::setTurnSpeed(float _targetTurn)
{
  targetTurn = _targetTurn;
}

// turn degree
int wheel::setSteerHeading(int _targetTurnDegree, float _targetTurn,
			    float _targetSpeed)
{
  int initialHeading = aGyro->getSteerHeading();
  int finalHeading = initialHeading + _targetTurnDegree;
  Error2(ErrorWarning,"ini %d, final %d",initialHeading,finalHeading);
  int temp = initialHeading;
  targetSpeed = _targetSpeed;
  targetTurn = _targetTurn;
  while(temp > finalHeading)  {
    temp = aGyro->getSteerHeading();
    targetSpeed = _targetSpeed;
    targetTurn = _targetTurn;
    taskDelay(1);
  }
  Error2(ErrorWarning,"ini %d, final %d",initialHeading,temp);
  targetSpeed = 0;
  targetTurn = 0;

}

// get steer heading
int wheel::getSteerHeading()
{
  return aGyro->getSteerHeading();
}

void wheel::scanTask(int arg1, int arg2, int arg3, int arg4, int arg5,
		    int arg6, int arg7, int arg8, int arg9, int arg10)
{
    int 	distLeft, distRight;
    float	tSpeed, tTurn;
  while(!scanTaskQuit) {
    //
    // compute target direction and speed
    //

    tSpeed = targetSpeed;
    tTurn = targetTurn;

    Error2(ErrorNone, "targetSpeed=%f, targetTurn=%f",
				    tSpeed, tTurn);

    if (fabs(tSpeed-lastSpeed)>maxSpeedDiff) {
      if (tSpeed>lastSpeed)
	tSpeed=lastSpeed+maxSpeedDiff;
      else
	tSpeed=lastSpeed-maxSpeedDiff;
    }

    if (fabs(tTurn-lastTurn)>maxTurnDiff) {
      if (tTurn>lastTurn)
	tTurn=lastTurn+maxTurnDiff;
      else
	tTurn=lastTurn-maxTurnDiff;
    }

    lastSpeed=tSpeed;
    lastTurn=tTurn;

    distLeft 	= (int)((tSpeed+tTurn)*SpeedScaler+0.5);
    distRight	= (int)((tSpeed-tTurn)*SpeedScaler+0.5);

#ifdef REAL_HARDWARE
    Error2(ErrorNone, "distRight=%d, distLeft=%d", distRight, distLeft);
    speed_command(0, -distRight);
    speed_command(1, distLeft);
#endif

    taskDelay(delayTicks);
  }
  semGive(scanTaskSem);
  Error(ErrorDebug, "wheel::fire end");
}

//
// Module Interface Functions
//

int wheelInitialize(void)
{
  dspInit();
  speed_command(0, 0);
  speed_command(1, 0);
  return 0;
}

int wheelFinish(void)
{
  return 0;
}




//////////////////////// dsp board interface functions ////////////////////
extern "C" {
struct dsp_str *dsp = (struct dsp_str *) 0xfb000000;

void
setup_parm( int axis, double kp, double ki )
{
	dsp->speed[axis].mode = SETUP;
	dsp->speed[axis].kp = (int)(kp*1000.);
	dsp->speed[axis].ki = (int)(ki*1000.);
}

void
speed_command( int axis, int speed )		/* pulse per msec */
{
	dsp->speed[axis].speed = speed;
	dsp->speed[axis].mode = COMMAND;
}

int
get_speed( int axis )
{
	return dsp->speed[axis].cur_vel;
}

int
get_position( int axis )
{
	return dsp->speed[axis].cur_pos;
}

int
speed_loop()
{
  while(1) {
  speed_command(0, -10);
  speed_command(1,10);
  taskDelay(100);
  }
}

int
speed1()
{
  while(1) {
  speed_command(0,-10);
  taskDelay(100);
  }
}

int
speed2()
{
  while(1) {
  speed_command(1,-10);
  taskDelay(100);
  }
}

int
stop()
{
  speed_command(0,0);
  speed_command(1,0);
}
} // extern "C" {
