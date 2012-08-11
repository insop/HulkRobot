#define _MODULE_NAME_ circle

#include "dfl.h"
#include "mvPort.h"

#include "circle.h"

const float MaxSpeed=1.0;
const float TargetSpeed=0.1;

circle::circle(TModule *parentTask, char *name)
:DPU(parentTask, name)
{
  addOPort( speedOutput=new mvOPort(this, "Speed", MaxSpeed, -MaxSpeed));
  addOPort( turnOutput=new mvOPort(this, "Turn", MaxSpeed, -MaxSpeed));
  addOPort( emStopOutput=new OPort(this, "EmergencyStop", DataTypeInt));
}

circle::~circle()
{  
/* output ports deleted by ~DPU()
  delete speedOutput;
  delete turnOutput;
  delete emStopOutput;
*/
}

int circle::init()
{
  t=0;
  return 0;
}

int circle::fire()
{
  int tt;

  Error(ErrorDebug, "circle::fire()");

  speedOutput->clearRange();
  turnOutput->clearRange();

  emStopOutput->setIntValue(0);

  if (t<10) {
    speedOutput->setRangeValue( t*TargetSpeed/10.0, t*TargetSpeed/10.0, 10 );
    turnOutput->setRangeValue( 0.0, 0.0, 10 );
  } else {
    speedOutput->setRangeValue( TargetSpeed, TargetSpeed, 10 );
    //turnOutput->setRangeValue( 0.05, 0.05, 10 );
    turnOutput->setRangeValue( 0.00, 0.00, 10 );
  } 
  t++;
  return 0;
}
