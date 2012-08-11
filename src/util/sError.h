#ifndef _S_ERROR_H_
#define _S_ERROR_H_
#include <stdio.h>
#include "error.h"

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

#define sError(errorLevel, errorMsg) \
	{ if (errorLevel>=errorThreshold) \
	  ffprintf( "(%10s:%4d)\t%s\n", __FILE__, __LINE__, errorMsg ); \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define sError1(errorLevel, errorMsg, arg1) \
	{ if (errorLevel>=errorThreshold) { \
	    fprintf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    fprintf( errorMsg, arg1 ); \
	    fprintf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define sError2(errorLevel, errorMsg, arg1, arg2) \
	{ if (errorLevel>=errorThreshold) { \
	    fprintf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    fprintf( errorMsg, arg1, arg2 ); \
	    fprintf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define sError3(errorLevel, errorMsg, arg1, arg2, arg3) \
	{ if (errorLevel>=errorThreshold) {\
	    fprintf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    fprintf( errorMsg, arg1, arg2, arg3 ); \
	    fprintf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define sError4(errorLevel, errorMsg, arg1, arg2, arg3, arg4) \
	{ if (errorLevel>=errorThreshold) {\
	    fprintf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    fprintf( errorMsg, arg1, arg2, arg3, arg4 ); \
	    fprintf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define sError5(errorLevel, errorMsg, arg1, arg2, arg3, arg4, arg5) \
	{ if (errorLevel>=errorThreshold) { \
	    fprintf( "(%10s:%4d)\t", __FILE__, __LINE__ ); \
	    fprintf( errorMsg, arg1, arg2, arg3, arg4, arg5 ); \
	    printf( "\n" ); \
	  } \
	  if (errorLevel==ErrorExit) fatalExit(); }

#define sAssert(expr)\
	{if (!(expr)) Error(ErrorExit, "Assertion Error");}
#else
#define sError(errorLevel, errorMsg)
#define sError1(errorLevel, errorMsg, arg1)
#define sError2(errorLevel, errorMsg, arg1, arg2)
#define sError3(errorLevel, errorMsg, arg1, arg2, arg3)
#define sError4(errorLevel, errorMsg, arg1, arg2, arg3, arg4)
#define sError5(errorLevel, errorMsg, arg1, arg2, arg3, arg4, arg5)
#define sAssert(expr)
#endif

#endif // _S_ERROR_H_
