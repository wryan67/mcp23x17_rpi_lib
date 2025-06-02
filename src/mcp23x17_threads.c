#include "mcp23x17_threads.h"




static void (*isrFunctionsWithPin[256])(int);





/*
 * wiringPiISR:
 *	Take the details and create an interrupt handler that will do a call-
 *	back to the user supplied function.
 *********************************************************************************
 */


int mcp23x17_wiringPiISRWithPin(int pin, int mode, void (*function)()) {

    if ((pin < 0) || (pin > 63))
        return wiringPiFailure(WPI_FATAL, "wiringPiISR: pin must be 0-63 (%d)\n", pin);


    isrFunctionsWithPin[pin] = function;


    if (mcp23x17_getDebug()) {
        fprintf(stderr, "mcp23x17::PIN<%d,INPUT>;PUD(OFF);wiringPiISR(%d,%d,fun)\n",pin,pin,mode); fflush(stderr);
    }

    pinMode(pin,INPUT);
    pullUpDnControl(pin, PUD_OFF);
    wiringPiISR(pin,mode,function);

    return 0;

}


