//
// usonic.cc
//
// This module reads ultra sonic sensor value
//
// Originally written by Yunho Jeon
// Human Robot Project
// KIST
//
// Ultrasonic sensor board made by
// Insub Song
// KIST/Korea Univ.
//
// Modified by Insop Song
// 1997 11
// ver 0.5
//

#define _MODULE_NAME_ usonic
#include "dfl.h"
#include "ioLib.h"
#include "tyLib.h"
#include "stdio.h"
#include "usonic.h"

unsigned usonic::range[MaxSonars];
SEM_ID usonic::scanTaskSem;
int usonic::scanTaskId;
int usonic::scanTaskQuit;
int usonic::numOpen;

usonic::usonic()
{
  for(int i=0; i<MaxSonars;i++)
    range[i]=0x7fffffff; /* max value */
  if(numOpen==0) {
    createScanTask();
  }
  numOpen++;
}

usonic::~usonic()
{
  numOpen--;
  if(numOpen==0)
    terminateScanTask();
}

void usonic::createScanTask()
{
  scanTaskSem=semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
  scanTaskQuit=0;
  scanTaskId=taskSpawn("USonicTask", 0 /*50*/, 0, 64*1024, 
			(FUNCPTR) scanTask,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0);
  Error( ErrorDebug, "USonicTask spawned" );
}

void usonic::terminateScanTask()
{

  scanTaskQuit=1;
  // wait until scan task quits 
  if(semTake(scanTaskSem, OSTickPeriod*1000)==ERROR) {	// wait 1 sec
    // kill the task
    Error(ErrorWarning, "Terminating scan task failed");
    Error(ErrorWarning, "maybe usonic board is not responding?");
    if(taskDelete(scanTaskId)==ERROR) {
      Error(ErrorFatal, "Deleting scan task failed");
    }
  }
  semDelete(scanTaskSem);
  Error(ErrorDebug, "Scan task terminated" );
}

void usonic::scanTask(int arg1, int arg2, int arg3, int arg4, int arg5,
			int arg6, int arg7, int arg8, int arg9, int arg10)
{
  Error(ErrorDebug, "Scantask started" );

  FILE * usonicDev;
  int i, num, value;
  char buffer[256];
  char linebuffer[256];
  char ch, dummy;
  int counter=0;

  for(i=0;i<MaxSonars;i++)
    range[i]=0xffff;

  usonicDev=fopen("/tyCo/1", "r+");
  if (usonicDev==(FILE *)NULL) {
    Error(ErrorFatal, "Unable to open port for ultra sonic board");
    semGive(scanTaskSem);
    return;
  }

  if(ioctl(fileno(usonicDev), FIOBAUDRATE, 19200)==ERROR) {
    Error(ErrorFatal, "Unable to set baud rate");
    return;
  }

  setvbuf(usonicDev, linebuffer, _IOLBF, 256);

  fflush(usonicDev);
  buffer = "C 0 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 E";
  printf("%s \n", buffer);
  fprintf(usonicDev, "%s \n", buffer);


/*
  while(1) {
    ch=getc(usonicDev);
    printf( "%c", ch );
  }
*/


  while(!scanTaskQuit) {

    do {
      ch=getc(usonicDev);
    } while(ch!='S' && !scanTaskQuit);

    fread(buffer, 82, 1, usonicDev);
    buffer[82]=0;

    if (scanTaskQuit) break;
	
    char RangeBuffer[6];
    for(i=0; i<MaxSonars;i++) {
      memcpy(RangeBuffer, buffer+(i*5), 5);
      RangeBuffer[(i*5)+5]=0;
      value = atoi(RangeBuffer);
      range[i]=value;
    }
/*
    for(i=0;i<MaxSonars;i++) {
      value=hex2bin(buffer+i*4+2, 4);
      if (value!=-1)
	range[i]=value;
    }
*/
    if ((counter++)%10==0) {
    Error1( ErrorWarning, "%s", buffer );
    Error4( ErrorWarning, "%04d %04d %04d %04d",
			  range[0], range[1], 
			  range[2], range[3]);
    Error4( ErrorWarning, "%04d %04d %04d %04d",
			  range[4], range[5], 
			  range[6], range[7] );
    Error4( ErrorWarning, "%04d %04d %04d %04d",
			  range[8], range[9], 
			  range[10], range[11] );
    Error4( ErrorWarning, "%04d %04d %04d %04d",
			  range[12], range[13], 
			  range[14], range[15] );
    }
    
  }
  //for stop Normadic sensor
  //buffer = "C";
  //fprintf(usonicDev, "%s \n", buffer);

  fclose(usonicDev);
  semGive(scanTaskSem);
}
unsigned usonic::usData(int _num)
{
  return range[_num];
}


