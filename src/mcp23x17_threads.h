#pragma once
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>


#include <poll.h>
#include <unistd.h>



#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>




// methods
int piBoardRev(void);      // wiringPi

int mcp23x17_getDebug();   

int mcp23x17_wiringPiISRWithPin(int pin, int mode, void (*function)(int pin));
