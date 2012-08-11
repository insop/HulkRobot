#ifndef _ERROR_H_
#define _ERROR_H_
#include <stdio.h>

enum {
  ErrorNone,
  ErrorDebug,
  ErrorWarning,
  ErrorFatal,
  ErrorExit
};

void initError(int errorThreshold);
void fatalExit(void);
void atFatalExit(void (*exitFunc)(void));	// emergency halt function
						// registration

#ifdef DEBUG
extern int errorThreshold;

#define Error(errorLevel, errorMsg) \
	{ if (errorLevel>=errorThreshold) \
	  printf( "(%10s:%4d)\t%s\n", __FILE__, __LINE__, errorMsg ); \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define Error1(errorLevel, errorMsg, arg1) \
	{ if (errorLevel>=errorThreshold) { \
	    printf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    printf( errorMsg, arg1 ); \
	    printf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define Error2(errorLevel, errorMsg, arg1, arg2) \
	{ if (errorLevel>=errorThreshold) { \
	    printf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    printf( errorMsg, arg1, arg2 ); \
	    printf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define Error3(errorLevel, errorMsg, arg1, arg2, arg3) \
	{ if (errorLevel>=errorThreshold) {\
	    printf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    printf( errorMsg, arg1, arg2, arg3 ); \
	    printf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define Error4(errorLevel, errorMsg, arg1, arg2, arg3, arg4) \
	{ if (errorLevel>=errorThreshold) {\
	    printf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    printf( errorMsg, arg1, arg2, arg3, arg4 ); \
	    printf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define Error5(errorLevel, errorMsg, arg1, arg2, arg3, arg4, arg5) \
	{ if (errorLevel>=errorThreshold) { \
	    printf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    printf( errorMsg, arg1, arg2, arg3, arg4, arg5 ); \
	    printf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define Assert(expr)\
	{if (!(expr)) Error(ErrorExit, "Assertion Error");}
#else
#define Error(errorLevel, errorMsg)
#define Error1(errorLevel, errorMsg, arg1)
#define Error2(errorLevel, errorMsg, arg1, arg2)
#define Error3(errorLevel, errorMsg, arg1, arg2, arg3)
#define Error4(errorLevel, errorMsg, arg1, arg2, arg3, arg4)
#define Error5(errorLevel, errorMsg, arg1, arg2, arg3, arg4, arg5)
#define Assert(expr)
#endif

#endif // _ERROR_H_
