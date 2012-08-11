//
// File MonitorCorner.h
//
//
// Originally written by Yunho Jeon
// Division of Electronics and Information
// KIST
//
//

#ifndef _Mc_H_
#define _MC_H_

#include "usonic.h"
#include "gyro.h"
#include "wheel.h"


const int   THRESHOLDcorner = 130;
const int   upTHRESHOLDcorner = 250;
const int   endCorner = 90;

//
// MonitorCorner Class
//
class MonitorCorner {
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
  int		corner;
  int		detectedNumLower;
  int		detectedNumUpper;

  static unsigned range[16];
  static unsigned detected[8];

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

  static void taskModuleTickerStub(MonitorCorner *This, int d2, 
				int d3, int d4, int d5, int d6, 
				int d7, int d8, int d9, int d10);
  static void taskModuleStub(MonitorCorner *This, int d2, int d3, 
				int d4, int d5, int d6, int d7, 
				int d8, int d9, int d10);


public:
  MonitorCorner(int priority, int samplingPeriod, int stackSize,
		gyro *aG, usonic *aU, wheel *aW);
  ~MonitorCorner();

  int 	init(void);
  int 	step(void);
  int 	run(void);
  int 	stop(void);

  int   isCorner(void);
};

#endif // _MC_H_
