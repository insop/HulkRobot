//
// hulk.cc
//
// This is the main program of hulk robot
//
// Originally written by Yunho Jeon
// 
// Written by Insop Song
// Intelligent System Contrl Research Center
// KIST
//
// 1997 12 1
// ver0.1
//


#define FIRSTLEVEL
#define SECONDLEVEL
//#define THIRDLEVEL
//#define FOURTHLEVEL

#include <stdio.h>
#include "dfl.h"
#include "wheel.h"
#include "usonic.h"
#include "gyro.h"

#include "AvoidObstacle.h"
#include "FollowWall.h"
#include "AlignWall.h"
#include "GoStraight.h"
#include "TurnCorner.h"

#include "MonitorFront.h"
#include "MonitorSide.h"
#include "MonitorDoor.h"
#include "MonitorCorner.h"

void hulkmaincpp(void);
void runTest(wheel*);
void turnTest(gyro *, wheel*);

//int hulkSpeedFirst = 0;

extern "C" {

void hulkmain()
{
  hulkmaincpp(); 
}

void _exit()
{
  printf("_exit() called for unknown reason\n");
}

}	// extern "C"


void hulkmaincpp(void)
{
  printf( "Hulk booting\n" );
  initError(ErrorWarning);	// initialize error reporting module
				// and set error report threshold

  gyro *aGyro;
  aGyro= new gyro();

  usonic *aUSonic;
  aUSonic = new usonic();

  wheel 	*aWheel;
  aWheel = new wheel(10, aGyro);

  AvoidObstacle	 aAvoidObstacle(70, 500,200*1024, aGyro, aUSonic, aWheel);
  FollowWall	 aFollowWall(80, 500,200*1024, aGyro, aUSonic, aWheel);
  AlignWall	 aAlignWall(100, 500,200*1024, aGyro, aUSonic, aWheel);
  TurnCorner	 aTurnCorner(110, 500,200*1024, aGyro, aUSonic, aWheel);
  GoStraight	 aGoStraight(120, 500,64*1024, aGyro, aUSonic, aWheel);

  MonitorFront	 aMonitorFront(60, 500,64*1024, aGyro, aUSonic, aWheel);
  MonitorCorner  aMonitorCorner(40, 50,64*1024, aGyro, aUSonic, aWheel);
  MonitorDoor	 aMonitorDoor(50, 50,100*1024, aGyro, aUSonic, aWheel);
  MonitorSide	 aMonitorSide(90, 500,64*1024, aGyro, aUSonic, aWheel);


  printf( "Hulk Command Interpreter Running\n\n");
  printf( "Available commands: A(void) F(ollow) G(oStraight) R(un) T(urn) D(egree) S(top) Q(uit)\n" );
  int quit=0;
  int running=0;

  int tid;

  // for testing
/*
  while(1) {
    printf("Current steer heading -> %d\n",aWheel->getSteerHeading());
    taskDelay(2000);
  }
  */


  char buffer[80];
  while(!quit) {
    printf( "command:\n" );
    gets( buffer );
    switch( buffer[0] ) {
      case 'A':
      case 'a':
	if (!running) {
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  aAvoidObstacle.init();
	  aAvoidObstacle.run();
	  running=1;
	}
	break;

      case 'W':
      case 'w':
	if (!running) {
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  aAlignWall.init();
	  aAlignWall.run();
	  running=1;
	}
	break;
      case 'M':
      case 'm':
	if (!running) {
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  aMonitorFront.init();
	  aMonitorFront.run();
	  running=1;
	}
	break;

      case 'F':
      case 'f':
	if (!running) {
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  aFollowWall.init();
	  aFollowWall.run();
	  running=1;
	}
	break;
      case 'R':
      case 'r':
	if (!running) {
	  runTest(aWheel);
	  running=1;
	}
	break;
      case 'G':
      case 'g':
	if (!running) {
	  aGoStraight.init();
	  aGoStraight.run();
	  running=1;
	}
	break;

      case 'T':
      case 't':
	if (!running) {
	  turnTest(aGyro, aWheel);
	  running=1;
	}
	break;
      case 'D':
      case 'd':
	if (!running) {
	  aWheel->setSteerHeading(-900, -0.08);
	  running=1;
	}
	break;
      case 'S':
      case 's':
	if (running) {
	  aAvoidObstacle.stop();
	  running=0;
	}
	break;

      case 'Q':
      case 'q':
	if (running) { 
	  running=0;
	}
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	  taskDelay(1000);
	quit=1;
	break;
    }
  }

  // for MonitorFront and AvoidObstacle and FollowWall check

  aMonitorFront.init();
  aMonitorSide.init();
  aMonitorDoor.init();
  aMonitorCorner.init();

  aMonitorDoor.run();
  aMonitorFront.run();
  aMonitorSide.run();
  aMonitorCorner.run();

  //aFollowWall.run();

  while(1) {
    if((aMonitorDoor.isDoor()==1) && (aMonitorFront.isFront()==0)){
      Error(ErrorWarning,"Trasition to Go Straight");
      Error(ErrorWarning,"Door Detetected");
      aAvoidObstacle.stop();
      aFollowWall.stop();
      aAlignWall.stop();
      aTurnCorner.stop();

      aGoStraight.run();

    }
    else if((aMonitorCorner.isCorner()==1) && 
				    (aMonitorFront.isFront()==0)){
      Error(ErrorWarning,"Trasition to Turn Corner");
      Error(ErrorWarning,"Corner Detetected");
      aAvoidObstacle.stop();
      aFollowWall.stop();
      aAlignWall.stop();
      aGoStraight.stop();

      aTurnCorner.run();
    }
    else if(aMonitorFront.isFront()==1){
      Error(ErrorWarning,"Trasition to Avoid Obstacle");
      aGoStraight.stop();
      aFollowWall.stop();
      aAlignWall.stop();
      aTurnCorner.stop();

      aAvoidObstacle.run();

    }
    else if((aMonitorSide.isSide()==1)&& (aMonitorFront.isFront()==0)
     && (aMonitorDoor.isDoor()==0) && (aMonitorCorner.isCorner()==0) ){
      Error(ErrorWarning,"Trasition to Align Wall");
      aGoStraight.stop();
      aAvoidObstacle.stop();
      aFollowWall.stop();
      aTurnCorner.stop();

      aAlignWall.run();
    }
    else /*if((aMonitorSide.isSide()==0)(aMonitorFront.isFront()==0)*/{
      Error(ErrorWarning,"Trasition to Follow Wall");
      aGoStraight.stop();
      aAvoidObstacle.stop();
      aAlignWall.stop();
      aTurnCorner.stop();

      aFollowWall.run();

    }
    taskDelay(400);
  }

  delete aWheel;
  delete aGyro;
  delete aUSonic;
}


void	runTest(wheel *aW)
	
{
  wheel *aWheel;
  aWheel = aW;
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setTurnSpeed(0.0);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.0);
  aWheel->setTurnSpeed(0.0);
  /*
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setTurnSpeed(0.05);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setTurnSpeed(-0.1);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setTurnSpeed(0.1);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.0);
  aWheel->setTurnSpeed(0.0);
  */

}


void turnTest(gyro *aG, wheel *aW)
{
  wheel *aWheel;
  gyro  *aGyro;
  aWheel= aW;
  aGyro = aG;
  int initialHeading = aGyro->getSteerHeading();
  int finalHeading = initialHeading - 900;
  int temp;
  printf("init is %d final is %d\n ", initialHeading, finalHeading);

  aWheel->setSpeed(0.0);
  aWheel->setTurnSpeed(-0.08);
  while(temp > finalHeading) {
    temp = aGyro->getSteerHeading();
    aWheel->setSpeed(0.0);
    aWheel->setTurnSpeed(-0.08);
    taskDelay(1);
  }
  Error2(ErrorWarning,"currnet %d aim %d", temp, finalHeading);
  aWheel->setSpeed(0.0);
  aWheel->setTurnSpeed(0.0);

  printf("final is %d\n ", aGyro->getSteerHeading);


}
