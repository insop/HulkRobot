//
// robot.cc

//

#include <stdio.h>
#include "dfl.h"
#include "wheel.h"
#include "usonic.h"
#include "gyro.h"

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

  ////////////////////////////////////////////////////////////
  // 		for Gyro Sensor Test			    //
  ////////////////////////////////////////////////////////////

  gyro *aGyro;
  aGyro= new gyro();

/*
  while(1)
    printf("heading is %d ", aGyro->getSteerHeading();
  printf("Gyro testing \n");
  while(1);	// test;


*/

/*
  usonic *aUSonic;
  aUSonic = new usonic();

*/
  wheel 	*aWheel;
  aWheel = new wheel(100);

/*

  taskDelay(1000);
  aWheel->setSpeed(0.0);
  aWheel->setSteerHeading(0.05);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.01);
  aWheel->setSteerHeading(-0.05);
  aWheel->setSpeed(0.1);
  aWheel->setSteerHeading(-0.05);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setSteerHeading(0.2);
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setSteerHeading(-0.2);
  taskDelay(1000);
  aWheel->setSpeed(0.0);
  aWheel->setSteerHeading(0.0);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(-0.1);
  aWheel->setSteerHeading(0.0);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.0);
  aWheel->setSteerHeading(0.0);
*/

/*
  int initialHeading = aGyro->getSteerHeading();
  int finalHeading = initialHeading + 450;
  int temp;
  printf("init is %d final is %d\n ", initialHeading, finalHeading);

  aWheel->setSpeed(0.01);
  aWheel->setSteerHeading(-0.01);
  while(aGyro->getSteerHeading<finalHeading) {
    taskDelay(5);
  }
  aWheel->setSpeed(0.0);
  aWheel->setSteerHeading(0.0);

  printf("final is %d\n ", aGyro->getSteerHeading);
  */



  printf( "Hulk Command Interpreter Running\n\n");
  printf( "Available commands: R(un) S(top) Q(uit)\n" );
  int quit=0;
  int running=0;

  int tid;


  char buffer[80];
  while(!quit) {
    printf( "command:\n" );
    gets( buffer );
    switch( buffer[0] ) {
      case 'R':
      case 'r':
	if (!running) {
	  runTest(aWheel);
	  running=1;
	}
	break;

      case 'S':
      case 's':
	if (running) {
	  running=0;
	}
	break;

      case 'Q':
      case 'q':
	if (running) { 
	  running=0;
	}
	quit=1;
	break;
    }
  }
  delete aWheel;
  //delete aGyro;
}

void	runTest(wheel *aW)
	
{

  wheel *aWheel;
  aWheel = aW;
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setSteerHeading(0.0);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setSteerHeading(-0.05);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  /*
  aWheel->setSpeed(0.1);
  aWheel->setSteerHeading(-0.05);
  taskDelay(1000);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setSteerHeading(0.2);
  taskDelay(1000);
  aWheel->setSpeed(0.1);
  aWheel->setSteerHeading(-0.2);
  taskDelay(1000);
  aWheel->setSpeed(0.0);
  aWheel->setSteerHeading(0.0);
  taskDelay(1000);
  taskDelay(1000);
  aWheel->setSpeed(-0.1);
  aWheel->setSteerHeading(0.0);
  taskDelay(1000);
  taskDelay(1000);
  */
  aWheel->setSpeed(0.0);
  aWheel->setSteerHeading(0.0);
  delete aWheel;

}


