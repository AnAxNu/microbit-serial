#include "microbit-serial.h"
#include <stdio.h> //printf
#include <sys/time.h> //timing functions

//calculate time passed. Start with doWhat=0 and then get time passed with doWhat=1
const double timing(int doWhat=0) {
  static timeval start_time;
  timeval curr_time;
  float rtn=0;

  if(doWhat==0) {
    //start timer
    gettimeofday(&start_time, NULL);
  }else if(doWhat==1) {
    //get time passed since start
    gettimeofday(&curr_time, NULL);
    rtn = (curr_time.tv_sec - start_time.tv_sec) * 1000.0;      // sec to ms
    rtn += (curr_time.tv_usec - start_time.tv_usec) / 1000.0;   // us to ms
  }
  
  return rtn;
}

int main (int argc, char **argv) {

  int x,y,z = 0;
  int status = 0;
  MicroBitSerial mbs;

  //init connection. Parameter is port to be used
  status = mbs.init(MicroBitSerial::SERIAL_PORT_DEFAULT_STRING);
  if(status != MicroBitSerial::SUCCESS) {
    printf("Failed to init MicroBitSerial. Error code: %i \r\n",status);
    return 0;
  }


  //get version info from the controller
  char *pVersion = NULL;
  mbs.getVersion(&pVersion);
  printf("Microbit version string: %s\r\n", pVersion);
  if(pVersion != NULL) {
    free(pVersion);
  }


  //start timer
  timing(0);

  printf("\r\nStarting to read data from accelerometer\r\n");

  int loop = 10;
  for(loop=10;loop != 0; loop--) {
    status = mbs.readAccelerometer(&x,&y,&z);
    if(status == MicroBitSerial::SUCCESS) {
      printf("x,y,z : %i,%i,%i \r\n", x, y, z);
    }else{
      printf("Failed to read accelerometer data. Error code: %i \r\n",status);
      return 0;
    }
    printf("Time: %f ms.\n", timing(1));
  }

  printf("\r\nTest is finished.\r\n");

  return 0; // success
}