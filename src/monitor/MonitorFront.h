//
// File MonitorFront.h
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

#ifndef _MF_H_
#define _MF_H_

#include "usonic.h"
#include "gyro.h"
#include "wheel.h"


const int   FrontZone = 50;

//
// MonitorFront Class
//
class MonitorFront {
enum {
  TURN_HARD_RIGHT,
  TURN_HARD_LEFT,
  TURN_SOFT_RIGHT,
  TURN_SOFT_LEFT,
  GO,
  STOP,
  BACK
};
private:
  gyro		*aGyro;
  usonic	*aUSonic;
  wheel		*aWheel;
  int   	priority;
  int   	samplingPeriod;
  int		delayTicks;
  int		turnMode;
  int		front;

  static unsigned range[16];

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
  unsigned minimum(unsigned a, unsigned b, unsigned c);

  //
  // The stub functions to call nonstatic member function
  // from a newly created task 
  //

  static void taskModuleTickerStub(MonitorFront *This, int d2, 
				int d3, int d4, int d5, int d6, 
				int d7, int d8, int d9, int d10);
  static void taskModuleStub(MonitorFront *This, int d2, int d3, 
				int d4, int d5, int d6, int d7, 
				int d8, int d9, int d10);


public:
  MonitorFront(int priority, int samplingPeriod, int stackSize,
		gyro *aG, usonic *aU, wheel *aW);
  ~MonitorFront();

  int 	init(void);
  int 	step(void);
  int 	run(void);
  int 	stop(void);

  int   isFront(void);
};

#endif // _MF_H_
