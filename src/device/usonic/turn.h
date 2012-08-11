#ifndef _TURN_H_
#define _TURN_H_
#ifndef _ANDIPORT_H_
#include "andIPort.h"
#endif

class turn: public DPU
{
enum {
  TURN_LEFT,
  TURN_RIGHT,
  TURN_STOP
};
private:
  int 		threshold;
  int 		turnMode;
  andIPort *	input;
  mvOPort *	turnOutput;
  mvOPort *	speedOutput;

public:
  turn(TModule *parentTask, char *name, int threshold, char *mode);
  virtual ~turn();

  virtual char*	getClassName(void)		{ return "turnIM"; }
  virtual int	init(void);
  virtual int 	fire(void);
};

#endif // _TURN_H_
