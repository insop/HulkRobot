#include "string.h"
#include "dfl.h"
#include "mvPort.h"
#include "andIPort.h"

#include "turn.h"

turn::turn(TModule *parentTask, char *name, int _threshold, char *_mode)
:DPU(parentTask, name)
{
  threshold = _threshold;

  if (!strcmp(_mode, "left")) 		turnMode = TURN_LEFT;
  else if (!strcmp(_mode, "right")) 	turnMode = TURN_RIGHT;
  else if (!strcmp(_mode, "stop")) 	turnMode = TURN_STOP;
  else {
    Error(ErrorFatal, "mode must be 'left', 'right' or 'stop'" );
    return;
  }

  addIPort(input = new andIPort(this, "Range"));
  addOPort(turnOutput = new mvOPort(this, "Turn", 1.0, -1.0));
  addOPort(speedOutput = new mvOPort(this, "Speed", 1.0, -1.0));
}

turn::~turn(void)
{
}

int turn::init(void)
{
  return 0;
}

int turn::fire(void)
{
  turnOutput->clearRange();
  speedOutput->clearRange();

  if ((unsigned int)(input->getIntValue())<threshold) {
    switch(turnMode) {
      case TURN_LEFT:
	speedOutput->setRangeValue(0.0,0.0,20);
	turnOutput->setRangeValue(-0.05,-0.05, 20);  
	Error(ErrorDebug, "turn left");
	break;
      case TURN_RIGHT:
	speedOutput->setRangeValue(0.0,0.0,20);
	turnOutput->setRangeValue(0.05,0.05, 20);  
	Error(ErrorDebug, "turn right");
	break;
      case TURN_STOP:
	speedOutput->setRangeValue(0,0,25);
	turnOutput->setRangeValue(0.0,0.0, 25);  
	Error(ErrorDebug, "turn stop");
	break;
    }
  }
  return 0;
}
