#ifndef _WHEEL_H_
#define _WHEEL_H_

#include "gyro.h"

const float MaxSpeed = 1.0;	// 1.0 m/s
const float MaxTurn  = 1.0;	// 1.0 m/s and -1.0 m/s for each wheel
const float MaxAcc   = 0.2;	// 0.1 m/s^2
const float MaxRAcc  = 0.2;	// rotational
const float SpeedScaler = 150.0;

class wheel
{
private:
  static float maxSpeedDiff;
  static float maxTurnDiff;
  static float lastSpeed;
  static float lastTurn;
  
  static int delayTicks;
  static int numOpen;

  static volatile float targetSpeed, targetTurn;
  unsigned int 	currentBlock;	
  static gyro	*aGyro;
  int	lastTurnDegree;

  static SEM_ID scanTaskSem;
  static volatile int scanTaskQuit;
  static int scanTaskId;
  static void scanTask(int, int, int, int, int,		
		  int, int, int, int, int);
  
  void	createScanTask();
  void	terminateScanTask();
  

public:
  wheel(gyro *aG, int samplingPeriod);
  static int setSpeed(float _targetSpeed);
  static int setTurnSpeed(float _targetTurn);
  static int setSteerHeading(int _targetTurnDegree, float _targetTurn,
					float _targetSpeed=0.0);
  virtual ~wheel();
};

#endif // _WHEEL_H_
