#define _MODULE_NAME_ wtest

#include "dfl.h"
#include "mvPort.h"

#include "wtest.h"

const float MaxSpeed=1.0;
const float TargetSpeed=0.1;

wtest::wtest(TModule *parentTask, char *name)
:DPU(parentTask, name)
{
  addOPort( speedOutput=new mvOPort(this, "Speed", MaxSpeed, -MaxSpeed));
  addOPort( turnOutput=new mvOPort(this, "Turn", MaxSpeed, -MaxSpeed));
  addOPort( emStopOutput=new OPort(this, "EmergencyStop", DataTypeSByte));
}

wtest::~wtest()
{  
/* output ports deleted by ~DPU()
  delete speedOutput;
  delete turnOutput;
  delete emStopOutput;
*/
}

int wtest::init()
{
  t=0;
  return 0;
}

int wtest::fire()
{
  int tt;

  Error(ErrorDebug, "wtest::fire()");

  speedOutput->clearRange();
  turnOutput->clearRange();

  emStopOutput->setSByteValue(0);
  tt=t%100;

  if (tt<10) {
    speedOutput->setRangeValue( tt*TargetSpeed/10.0, tt*TargetSpeed/10.0, 10 );
    turnOutput->setRangeValue( 0.0, 0.0, 10 );
  } else if (tt>=10 && tt<=90) {
    speedOutput->setRangeValue( 10.0*TargetSpeed/10.0, 10.0*TargetSpeed/10.0, 10 );
    turnOutput->setRangeValue( 0.07, 0.07, 10 );
  } else if (tt>=190 && tt<=200) {
    speedOutput->setRangeValue( 
      (100-tt)*TargetSpeed/10.0, (100-tt)*TargetSpeed/10.0, 10 );
    turnOutput->setRangeValue( 0.0, 0.0, 10 );
  } else {
    speedOutput->setRangeValue( 0.0, 0.0, 10 );
    turnOutput->setRangeValue( 0.0, 0.0, 10 );
  } 
  t++;
  return 0;
}

