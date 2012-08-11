#ifndef _WHEEL_H_
#define _WHEEL_H_

#include "gyro.h"

#define		RIGHTWHEEL	0
#define		RIGHTFWDIR	-1
#define		RIGHTBWDIR	+1
#define		LEFTWHEEL	1
#define		LEFTFWDIR	+1
#define		LEFTBWDIR	-1


const float MaxSpeed = 1.0;	// 1.0 m/s
const float MaxTurn  = 1.0;	// 1.0 m/s and -1.0 m/s for each wheel
const float MaxAcc   = 0.2;	// 0.1 m/s^2
const float MaxRAcc  = 0.2;	// rotational
const float SpeedScaler = 150.0;

const float M_PI = 3.141592654;	
const float RD_WHEEL = 0.102;	// m
const float RW_RADIUS = 0.100;	// m
const float LW_RADIUS = 0.104;	// m
const float N_GEAR = 36.0;
const float ENC_REV = 2000.0;
const float ENC2M = (2*M_PI*RD_WHEEL)/(ENC_REV*N_GEAR);
const float R_ENC2M = (2*M_PI*RW_RADIUS)/(ENC_REV*N_GEAR);
const float L_ENC2M = (2*M_PI*LW_RADIUS)/(ENC_REV*N_GEAR);
const float VM2ENC = (1./(ENC2M*1000.));	// 1[m/sec]= 104.174[pul/ms]
const float WHEELBASE = 0.2325;		// not phisical value(0.465/2
					// but calibrated value by experiment



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
  wheel(int samplingPeriod, gyro *aG);
  static int setSpeed(float _targetSpeed);
  static int setTurnSpeed(float _targetTurn);
  static int getSteerHeading();
  static int setSteerHeading(int _targetTurnDegree, float _targetTurn,
					float _targetSpeed=0.0);
  virtual ~wheel();
};

#endif // _WHEEL_H_
