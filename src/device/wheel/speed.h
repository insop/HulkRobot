#define RUN			0
#define SETUP		1
#define COMMAND	2

struct speed_parm_struct {
	int mode;
	int kp;			/* Kp gain = kp / 1000.0 */
	int ki;			/* Ki gain = ki / 1000.0 */
	int speed;
	int cur_pos;
	int cur_vel;
};

struct dsp_str {
	struct speed_parm_struct speed[2];
};


extern struct dsp_str *dsp;

void setup_parm( int axis, double kp, double ki );
void speed_command( int axis, int speed );		/* pulse per msec */

