//
// gyro.cc
//
// This module reads gyro sensor value
//
// Orignally written by Yunho Jeon
// Human Robot Project
// KIST
// 
// Modified by Insop Song
// 1997 12 1 
// ver 0.5

#define _MODULE_NAME_ gyro
#include "dfl.h"
#include "ioLib.h"
#include "tyLib.h"
#include "stdio.h"
#include "gyro.h"

int gyro::azimuth;
int gyro::numOpen;
SEM_ID gyro::scanTaskSem;
int gyro::scanTaskId;
int gyro::scanTaskQuit;

gyro::gyro()
{
  if(numOpen==0)  {
    createScanTask();
  }
  numOpen++;
  Error( ErrorDebug, "GyroTask Constructed" );
}

gyro::~gyro()
{
  numOpen--;
  if(numOpen==0)
    terminateScanTask();
}

void gyro::createScanTask()
{
  scanTaskSem=semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
  scanTaskQuit=0;
  scanTaskId=taskSpawn("GyroTask", 20 /*50*/, 0, 64*1024, 
			(FUNCPTR) scanTask,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0);
  Error( ErrorDebug, "GyroTask spawned" );
}

void gyro::terminateScanTask()
{
  scanTaskQuit=1;
  // wait until scan task quits 
  if(semTake(scanTaskSem, OSTickPeriod*1000)==ERROR) {	// wait 1 sec
    // kill the task
    Error(ErrorWarning, "Terminating scan task failed");
    Error(ErrorWarning, "maybe gyro sensor system is not responding?");
    if(taskDelete(scanTaskId)==ERROR) {
      Error(ErrorFatal, "Deleting scan task failed");
    }
  }
  semDelete(scanTaskSem);
  Error(ErrorDebug, "Scan task terminated" );
}

void gyro::scanTask(int arg1, int arg2, int arg3, int arg4, int arg5,
			int arg6, int arg7, int arg8, int arg9, int arg10)
{
  FILE * gyroDev;
  int i, num, value;
  char buffer[256];
  char linebuffer[256];
  char ch, dummy;
  int counter=0;
  int	status;

  gyroDev=fopen("/tyCo/2", "r");
  if (gyroDev==(FILE *)NULL) {
    Error(ErrorFatal, "Unable to open port for gyro sensor");
    semGive(scanTaskSem);
    return;
  }

  if(ioctl(fileno(gyroDev), FIOBAUDRATE, 9600)==ERROR) {
    Error(ErrorFatal, "Unable to set baud rate");
    return;
  }

  setvbuf(gyroDev, linebuffer, _IOLBF, 256);

  while(!scanTaskQuit) {

    do {
      ch=getc(gyroDev);
    } while(ch!='%' && !scanTaskQuit);

    fread(buffer, 15, 1, gyroDev);
    buffer[15]=0;
    if (scanTaskQuit) break;
	
    for(int i=0, found=0; i< 20 && found != 2; i++) {
      if( buffer[i] == ',') found++;
      if( found == 2) azimuth = atoi(buffer+(i+1));
    }
    if((counter++)%5==0) 
      Error1( ErrorDebug, "%d", azimuth );
  }
  fclose(gyroDev);
  semGive(scanTaskSem);
}

int gyro::getSteerHeading(void)
{
   return azimuth;
}

unsigned gyro::hex2bin(char *str, int length)
{
  unsigned sum=0;

  for(int i=0;i<length;i++, str++) {
    if (*str>='0' && *str<='9') 
      sum = (sum<<4) + *str - '0';
    else if (*str>='a' && *str<='f')
      sum = (sum<<4) + *str - 'a' + 10;
    else if (*str>='A' && *str<='F')
      sum = (sum<<4) + *str - 'A' + 10;
    else {
    /*
      Error1( ErrorWarning, "Bad character from ultra sonic board[%c]", *str );
    */
      // ignore bad character
      return 0xffffffff;
    }
  }
  return sum;
}


