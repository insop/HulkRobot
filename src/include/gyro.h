#ifndef _GYRO_H_
#define _GYRO_H_

class gyro
{
private:
  // shared among all instances
  static int 	azimuth;
  static int 	numOpen;
  static int	scanTaskId;
  static void 	scanTask(int, int, int, int, int,
			int, int, int, int, int);
  static SEM_ID	scanTaskSem;
  static volatile int	scanTaskQuit;

  static unsigned hex2bin(char *str, int length);

  void 		createScanTask();
  void		terminateScanTask();


public:
  gyro();
  virtual ~gyro();
  int getSteerHeading();
};

#endif // _GYRO_H_
