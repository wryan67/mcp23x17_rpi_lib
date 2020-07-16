#include "mcp23x17_threads.h"


volatile int mcp23x17_sysFds[64] =
{
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

static void (*isrFunctionsWithPin[256])(int);



int mcp23x17_waitForInterruptWithPin(int pin, int mS) {
    int fd, x;
    unsigned char c;

    struct pollfd polls;

    int bcmPin = wpiPinToGpio(pin);

    if ((fd = mcp23x17_sysFds[bcmPin]) == -1)
        return -2;

    // Setup poll structure

    polls.fd = fd;
    polls.events = POLLPRI | POLLERR;	// Urgent data!

  // Wait for it ...

    x = poll(&polls, 1, mS);

    // If no error, do a dummy read to clear the interrupt
    //	A one character read appars to be enough.

    if (x > 0) {
        lseek(fd, 0, SEEK_SET);	// Rewind
        (void)read(fd, &c, 1);	// Read & clear
    }


    return x;
}


/*
 * interruptHandler:
 *	This is a thread and gets started to wait for the interrupt we're
 *	hoping to catch. It will call the user-function when the interrupt
 *	fires.
 *********************************************************************************
 */

static void* mcp23x17_interruptHandlerWithPin(void* args) {
    int pin;

    memcpy(&pin, args, sizeof(int));

    if (mcp23x17_getDebug()) {
        fprintf(stderr, "mcp23x17_interruptHandlerWithPin::free--args %p\n", args); fflush(stderr);
    }
    free(args);


    (void)piHiPri(55);

    if (mcp23x17_getDebug()) {
        fprintf(stderr,"interruptHandlerWithPin pin=%d\n", pin); fflush(stderr);
    }

    for (int i=1;;++i) {
        if (mcp23x17_waitForInterruptWithPin(pin, -1) > 0) {
            if (mcp23x17_getDebug()) {
                fprintf(stderr, "caught interrupt on pin=%d\n", pin); fflush(stderr);
            }

            isrFunctionsWithPin[pin](pin);
        }
    }

    return NULL;
}

/*
 * wiringPiISR:
 *	Take the details and create an interrupt handler that will do a call-
 *	back to the user supplied function.
 *********************************************************************************
 */


int mcp23x17_wiringPiISRWithPin(int pin, int mode, void (*function)(int pin)) {
    pthread_t threadId;
    const char* modeS;
    char fName[64];
    char  pinS[8];
    pid_t pid;
    int   count, i;
    char  c;
    int   bcmGpioPin;


    if ((pin < 0) || (pin > 63))
        return wiringPiFailure(WPI_FATAL, "wiringPiISR: pin must be 0-63 (%d)\n", pin);


    bcmGpioPin = wpiPinToGpio(pin);

    if (mcp23x17_getDebug()) {
        fprintf(stderr, "setting isr on gpio pin = %d; bcm pin=%d\n", pin, bcmGpioPin);  fflush(stderr);
    }


    if (mode != INT_EDGE_SETUP) {
        /**/ if (mode == INT_EDGE_FALLING)
            modeS = "falling";
        else if (mode == INT_EDGE_RISING)
            modeS = "rising";
        else
            modeS = "both";

        sprintf(pinS, "%d", bcmGpioPin);

        if ((pid = fork()) < 0)  // Fail
            return wiringPiFailure(WPI_FATAL, "wiringPiISRpassPin: fork failed: %s\n", strerror(errno));

        if (pid == 0) // Child, exec
        {
            /**/ if (access("/usr/local/bin/gpio", X_OK) == 0) {
                execl("/usr/local/bin/gpio", "gpio", "edge", pinS, modeS, (char*)NULL);
                return wiringPiFailure(WPI_FATAL, "wiringPiISR: execl failed: %s\n", strerror(errno));
            }
            else if (access("/usr/bin/gpio", X_OK) == 0) {
                execl("/usr/bin/gpio", "gpio", "edge", pinS, modeS, (char*)NULL);
                return wiringPiFailure(WPI_FATAL, "wiringPiISR: execl failed: %s\n", strerror(errno));
            }
            else
                return wiringPiFailure(WPI_FATAL, "wiringPiISR: Can't find gpio program\n");
        }
        else    // Parent, wait
            wait(NULL);
    }


    // Now pre-open the /sys/class node - but it may already be open if
    //  we are in Sys mode...

    if (mcp23x17_sysFds[bcmGpioPin] == -1) {
        sprintf(fName, "/sys/class/gpio/gpio%d/value", bcmGpioPin);
        if ((mcp23x17_sysFds[bcmGpioPin] = open(fName, O_RDWR)) < 0) {
            fprintf(stderr, "failed to open %s\n", fName); fflush(stderr);
            exit(9);
//            return wiringPiFailure(WPI_FATAL, "wiringPiISR: unable to open %s: %s\n", fName, strerror(errno));
        }
    }

    // Clear any initial pending interrupt
    ioctl(mcp23x17_sysFds[bcmGpioPin], FIONREAD, &count);
    for (i = 0; i < count; ++i)
        read(mcp23x17_sysFds[bcmGpioPin], &c, 1);


    isrFunctionsWithPin[pin] = function;


    int *permPin = malloc(sizeof(int));
        *permPin = pin;


    if (mcp23x17_getDebug()) {
        fprintf(stderr, "<<mcp23x17_wiringPiISRWithPin::alloc--args %p\n", permPin); fflush(stderr);
    }
    pthread_create(&threadId, NULL, mcp23x17_interruptHandlerWithPin, (void*)(permPin));
    pthread_detach(threadId);


    return 0;

}


