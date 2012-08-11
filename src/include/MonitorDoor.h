//
// File MonitorDoor.h
//
//
// Originally written by Yunho Jeon
// Division of Electronics and Information
// KIST
//
//

#ifndef _MD_H_
#define _MD_H_

#include "usonic.h"
#include "gyro.h"
#include "wheel.h"


const int   THRESHOLD = 130;
const int   upTHRESHOLD = 250;
const int   endDOOR = 70;

//
// MonitorDoor Class
//
class MonitorDoor {
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
  int		door;
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

  static void taskModuleTickerStub(MonitorDoor *This, int d2, 
				int d3, int d4, int d5, int d6, 
				int d7, int d8, int d9, int d10);
  static void taskModuleStub(MonitorDoor *This, int d2, int d3, 
				int d4, int d5, int d6, int d7, 
				int d8, int d9, int d10);


public:
  MonitorDoor(int priority, int samplingPeriod, int stackSize,
		gyro *aG, usonic *aU, wheel *aW);
  ~MonitorDoor();

  int 	init(void);
  int 	step(void);
  int 	run(void);
  int 	stop(void);

  int   isDoor(void);
};

#endif // _MD_H_
