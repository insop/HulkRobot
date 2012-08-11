
class circle: public DPU {

private:
  int t;
  mvOPort *speedOutput, *turnOutput; 
  OPort *emStopOutput;

public:
  circle(TModule *parentTask, char *name);
  virtual ~circle();

  virtual char *getClassName(void)	{ return "circle"; }
  virtual int 	init(void);
  virtual int 	fire(void);
};
