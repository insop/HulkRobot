
class wtest: public DPU {

private:
  int t;
  mvOPort *speedOutput, *turnOutput; 
  OPort *emStopOutput;

public:
  wtest(TModule *parentTask, char *name);
  virtual ~wtest();

  virtual char *getClassName(void)	{ return "wtest"; }
  virtual int 	init(void);
  virtual int 	fire(void);
};
