#ifndef _USONIC_H_
#define _USONIC_H_

const int MaxSonars = 16;

class usonic
{
private:
  // shared among all instances
  static unsigned range[MaxSonars]; 
  static int	scanTaskId;
  static int	numOpen;
  static void 	scanTask(int, int, int, int, int,
			int, int, int, int, int);
  static SEM_ID	scanTaskSem;
  static volatile int	scanTaskQuit;

  void 		createScanTask();
  void		terminateScanTask();

public:
  usonic();
  ~usonic();
  unsigned usData(int);
};

#endif // _USONIC_H_
