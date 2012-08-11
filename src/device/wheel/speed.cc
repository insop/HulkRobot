#include "speed.h"

struct dsp_str *dsp = (struct dsp_str *) 0xfb000000;

void
setup_parm( int axis, double kp, double ki )
{
	dsp->speed[axis].mode = SETUP;
	dsp->speed[axis].kp = (int)(kp*1000.);
	dsp->speed[axis].ki = (int)(ki*1000.);
}

void
speed_command( int axis, int speed )		/* pulse per msec */
{
	dsp->speed[axis].mode = COMMAND;
	dsp->speed[axis].speed = speed;
}

int 
get_speed( int axis )
{
	return dsp->speed[axis].cur_vel;
}
