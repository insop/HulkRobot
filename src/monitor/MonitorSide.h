//
// File MonitorSide.h
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

#ifndef _MS_H_
#define _MS_H_

#include "usonic.h"
#include "gyro.h"
#include "wheel.h"


const int   SideZone = 50;
const int   SideMaxZone = 200;

//
// MonitorSide Class
//
class MonitorSide {
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
  int		side;

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

  static void taskModuleTickerStub(MonitorSide *This, int d2, 
				int d3, int d4, int d5, int d6, 
				int d7, int d8, int d9, int d10);
  static void taskModuleStub(MonitorSide *This, int d2, int d3, 
				int d4, int d5, int d6, int d7, 
				int d8, int d9, int d10);


public:
  MonitorSide(int priority, int samplingPeriod, int stackSize,
		gyro *aG, usonic *aU, wheel *aW);
  ~MonitorSide();

  int 	init(void);
  int 	step(void);
  int 	run(void);
  int 	stop(void);

  int   isSide(void);
};

#endif // _MS_H_
