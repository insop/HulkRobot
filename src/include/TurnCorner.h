//
// File TurnCorner.h
//
//
// Written by Yunho Jeon
// Division of Electronics and Information
// KIST
//

//
// This file contains class definitions and constants values for
// data flow layer of human robot control architecture
//

#ifndef _TC_H
#define _TC_H

#include <stdlib.h>
#include "gyro.h"
#include "wheel.h"
#include "usonic.h"


				// neighborhood sensor

//
// TurnCorner Class
//
class TurnCorner {
enum{
  TURN_RIGHT,
  TURN_LEFT,
  TURN_STOP,
  TURN_STOP_SLOW
};

private:
  gyro		*aGyro;
  usonic	*aUSonic;
  wheel		*aWheel;
  int   	priority;
  int   	samplingPeriod;
  int		delayTicks;
  int		turnMode;

  static unsigned forward;
  static unsigned middle;
  static unsigned reward;
  static unsigned difference;

  int		isTurn;	// turn check flag;
  int		isBack;	// back check flag;

  int		taskId, tickerTaskId; 
  SEM_ID	tickerSem, taskSem, tickerExitSem, taskExitSem;

  enum {
    TaskStopped,
    TaskPaused,
    TaskStep,
    TaskRunning,
    TaskTerminate
  };
  volatile int	taskMode;

  void 	task(void);
  void	ticker(void);

  //
  // The stub functions to call nonstatic member function
  // from a newly created task 
  //

  static void taskModuleTickerStub(TurnCorner *This, int d2, 
				int d3, int d4, int d5, int d6, 
				int d7, int d8, int d9, int d10);
  static void taskModuleStub(TurnCorner *This, int d2, int d3, 
				int d4, int d5, int d6, int d7, 
				int d8, int d9, int d10);


public:
  TurnCorner(int priority, int samplingPeriod, int stackSize,
		gyro *aG, usonic *aU, wheel *aW);
  ~TurnCorner();

  int 	init(void);
  int 	step(void);
  int 	run(void);
  int 	stop(void);
};

#endif // _TC_H
