#include "mcp23x17.h"
#include <sys/time.h>

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

int mcp23x17_handle = -1;
int mcp23x17_address = 0x20;
int mcp23x17_inta_pin = 27;
int mcp23x17_intb_pin = 28;

// mcp23017 pins
// Port-B
MCP23x17_GPIO intersectionLED = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 0);
MCP23x17_GPIO tram1Forward = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 1);
MCP23x17_GPIO tram1Reverse = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 2);
MCP23x17_GPIO tram2Forward = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 3);
MCP23x17_GPIO tram2Reverse = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 4);
MCP23x17_GPIO eastEOLPin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 5);
MCP23x17_GPIO eastGatePin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 6);
MCP23x17_GPIO northGatePin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 7);

// Port-A
MCP23x17_GPIO tramsEnabledLED = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 7);
MCP23x17_GPIO startStopButtonPin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 6);
MCP23x17_GPIO nc1 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 5);
MCP23x17_GPIO nc2 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 4);
MCP23x17_GPIO streetStopPin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 3);
MCP23x17_GPIO westEOLPin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 2);
MCP23x17_GPIO southGatePin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 1);
MCP23x17_GPIO westGatePin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 0);


#define STOP    -1
#define FORWARD  0
#define REVERSE  1

enum Trams
{
    TRAM1 = 0,   // north/south (circuit)
    TRAM2 = 1,   // east/west
    _TRAMS
};

enum Gates
{
    GATE1 = 0,
    GATE2 = 1,
    _GATES
};

enum Directions
{
    NORTH,
    SOUTH,
    EAST,
    WEST
};


volatile bool tramsEnabled = false;
volatile bool intersectionBlocked = false;
volatile int  tramInIntersection;
volatile int  exitGate[_TRAMS];
volatile int  exitCounter;

volatile bool ignoreNextStreetStop;
volatile int  tramDirections[_TRAMS];
volatile int  idleTramDirections[_TRAMS];
volatile int  lastEOL;

void moveTram(int tram, int direction);
void reverseTram(int tram);

unsigned long long currentTimeMillis() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    return
        (unsigned long long)(currentTime.tv_sec) * 1000 +
        (unsigned long long)(currentTime.tv_usec) / 1000;
}

void blockIntersection(Trams tram, Gates gate) {
    intersectionBlocked = true;
    printf("tram %d is blocking the intersection\n", tram + 1);
    tramInIntersection = tram;

    mcp23x17_digitalWrite(intersectionLED, 1);
    exitGate[tram] = !gate;
    exitCounter = 2;

}

void intersection(Trams tram, Gates gate) {
    if (!intersectionBlocked) {
        blockIntersection(tram, gate);
        return;
    }
    if (tram == tramInIntersection) {
        if (gate == exitGate[tram]) {
            if (--exitCounter < 1) {
                printf("unblocking intersection\n");
                intersectionBlocked = false;
                mcp23x17_digitalWrite(intersectionLED, 0);
            }
        }
        return;
    }

    printf("waiting for intersection to clear; tram %d is blocking it\n", 1 + tramInIntersection);

    int lastDirection = tramDirections[tram];
    moveTram(tram, STOP);
    while (intersectionBlocked) {
        delay(10);
    }

    blockIntersection(tram, gate);
    moveTram(tram, lastDirection);
}


void unidentifiedGate(int port, int pin, int value) {
    printf("unidentified gate pin-%d tripped with value %d; iStat=%d occured at %llu ms\n", pin, value, intersectionBlocked, currentTimeMillis()); 	fflush(stdout);
}


void startTrams() {
    int value;
    tramsEnabled = true;
    mcp23x17_digitalWrite(tramsEnabledLED, HIGH);

    if (idleTramDirections[TRAM1] == STOP) {
        idleTramDirections[TRAM1] = FORWARD;
    }
    if (idleTramDirections[TRAM2] == STOP) {
        idleTramDirections[TRAM2] = FORWARD;
    }


    value = mcp23x17_virtualRead(streetStopPin);
    if (value == 0) {
        ignoreNextStreetStop = true;
    }

    value = mcp23x17_virtualRead(westEOLPin);
    if (value == 0) {
        idleTramDirections[TRAM2] = REVERSE;
    }



    moveTram(TRAM1, idleTramDirections[TRAM1]);
    moveTram(TRAM2, idleTramDirections[TRAM2]);
}

void stopTrams() {
    tramsEnabled = false;
    mcp23x17_digitalWrite(tramsEnabledLED, LOW);

    idleTramDirections[TRAM1] = tramDirections[TRAM1];
    idleTramDirections[TRAM2] = tramDirections[TRAM2];

    moveTram(TRAM1, STOP);
    moveTram(TRAM2, STOP);
}


void startStopButton(int port, int pin, int value) {
    if (value != 0) {
        return;
    }
    if (tramsEnabled) {
        bool blockDelay = intersectionBlocked;
        while (intersectionBlocked) {
            delay(1);
        }
        if (blockDelay) {
            delay(100);
        }
        stopTrams();
    }
    else {
        startTrams();
    }

    printf("button pressed pin-%d tripped with value %d; iStat=%d occured at %llu ms\n", pin, value, intersectionBlocked, currentTimeMillis()); 	fflush(stdout);
}


void eastEOL(int port, int pin, int value) {
    if (value != 0) {
        return;
    }
    if (lastEOL == EAST) {
        return;
    }
    printf("east-eol  gate pin-%d tripped with value %d; iStat=%d occured at %llu ms\n", pin, value, intersectionBlocked, currentTimeMillis()); 	fflush(stdout);

    lastEOL = EAST;
    reverseTram(TRAM2);
}

void westEOL(int port, int pin, int value) {
    if (value != 0) {
        return;
    }
    if (lastEOL == WEST) {
        return;
    }
    printf("west-eol  gate pin-%d tripped with value %d; iStat=%d occured at %llu ms\n", pin, value, intersectionBlocked, currentTimeMillis()); 	fflush(stdout);

    lastEOL = WEST;
    reverseTram(TRAM2);
}



void eastGate(int port, int pin, int value) {
    if (value != 0) {
        return;
    }
    printf("east  gate pin-%d tripped with value %d; iStat=%d occured at %llu ms\n", pin, value, intersectionBlocked, currentTimeMillis()); 	fflush(stdout);
    intersection(TRAM2, GATE1);
}

void westGate(int port, int pin, int value) {
    if (value != 0) {
        return;
    }
    printf("west  gate pin-%d tripped with value %d; iStat=%d occured at %llu ms\n", pin, value, intersectionBlocked, currentTimeMillis()); 	fflush(stdout);
    intersection(TRAM2, GATE2);
}



void southGate(int port, int pin, int value) {
    if (value != 0) {
        return;
    }
    printf("south gate pin-%d tripped with value %d; iStat=%d occured at %llu ms\n", pin, value, intersectionBlocked, currentTimeMillis()); 	fflush(stdout);
    intersection(TRAM1, GATE1);
}

void northGate(int port, int pin, int value) {
    if (value != 0) {
        return;
    }
    printf("north gate pin-%d tripped with value %d; iStat=%d occured at %llu ms\n", pin, value, intersectionBlocked, currentTimeMillis()); 	fflush(stdout);
    intersection(TRAM1, GATE2);
}


void streetStop(int port, int pin, int value) {
    if (value != 0) {
        return;
    }

    if (!ignoreNextStreetStop) {
        printf("street stop pin-%d tripped with value %d; iStat=%d occured at %llu ms\n", pin, value, intersectionBlocked, currentTimeMillis()); 	fflush(stdout);

        int lastDirection = tramDirections[TRAM1];
        moveTram(TRAM1, STOP);
        delay(1 * 1000);
        if (tramsEnabled) {
            ignoreNextStreetStop = true;
            moveTram(TRAM1, lastDirection);
        }

    }
    else {
        ignoreNextStreetStop = false;
    }
}



int setup() {

    if (wiringPiSetup() != 0) {
        fprintf(stderr, "Wiring Pi could not be initialized\n");
        return 9;
    }

    mcp23x17_handle = mcp23x17_setup(0, mcp23x17_address, mcp23x17_inta_pin, mcp23x17_intb_pin);
    if (mcp23x17_handle < 0) {
        fprintf(stderr, "mcp23017 could not be initialized\n");
        return 9;
    }

    // identified
    mcp23x17_setPinOutputMode(intersectionLED, LOW);

    mcp23x17_setPinOutputMode(tramsEnabledLED, LOW);
    mcp23x17_setPinInputMode(startStopButtonPin, TRUE, startStopButton);

    mcp23x17_setPinInputMode(streetStopPin, TRUE, &streetStop);

    mcp23x17_setPinInputMode(southGatePin, TRUE, &southGate);
    mcp23x17_setPinInputMode(northGatePin, TRUE, &northGate);

    mcp23x17_setPinInputMode(westGatePin, TRUE, &westGate);
    mcp23x17_setPinInputMode(eastGatePin, TRUE, &eastGate);

    mcp23x17_setPinInputMode(westEOLPin, TRUE, &westEOL);
    mcp23x17_setPinInputMode(eastEOLPin, TRUE, &eastEOL);


    mcp23x17_setPinOutputMode(tram1Forward, LOW);
    mcp23x17_setPinOutputMode(tram1Reverse, LOW);
    mcp23x17_setPinOutputMode(tram2Forward, LOW);
    mcp23x17_setPinOutputMode(tram2Reverse, LOW);


    return 0;
}


void reverseTram(int tram) {
    int lastDirection = tramDirections[tram];
    moveTram(tram, STOP);
    delay(2 * 1000);
    if (tramsEnabled) {
        moveTram(tram, !lastDirection);
    }
}

void moveTram(int tram, int direction) {
    int forwardPin;
    int reversePin;

    switch (tram) {
    case TRAM1:
        forwardPin = tram1Forward;
        reversePin = tram1Reverse;
        break;
    case TRAM2:
        forwardPin = tram2Forward;
        reversePin = tram2Reverse;
        break;
    }

    switch (direction) {
    case FORWARD:
        mcp23x17_digitalWrite(forwardPin, HIGH);
        mcp23x17_digitalWrite(reversePin, LOW);
        break;
    case REVERSE:
        mcp23x17_digitalWrite(forwardPin, LOW);
        mcp23x17_digitalWrite(reversePin, HIGH);
        break;
    default:
        mcp23x17_digitalWrite(forwardPin, LOW);
        mcp23x17_digitalWrite(reversePin, LOW);
    }
    tramDirections[tram] = direction;

}

void* printStackTrace(void* args) {
    int   i;
    int   streamHandle;
    char  traceLine[4096];
    char  buf[4096];
    char  addr2lineBuf[4096];
    char  cmd[4096];
    char* addr;
    FILE* stream;
    FILE* addr2line;

    fprintf(stderr, "stack trace:\n"); fflush(stderr);

    memcpy(&streamHandle, args, sizeof(streamHandle));
    free(args);
    stream = fdopen(streamHandle, "r");

    while (fgets(traceLine, sizeof(traceLine), stream) != NULL) {
        if (0 == strcmp(traceLine, "---end---\n")) {
            fprintf(stderr, "%s", traceLine);
            break;
        }


        if (traceLine[strlen(traceLine) - 2] != ']') {
            fprintf(stderr, "trace---> %s", traceLine);
            continue;
        }

        strcpy(buf, traceLine);

        buf[strlen(buf) - 2] = 0;


        for (i = strlen(buf) - 1; i > 0; --i) {
            if (buf[i] == '[') {
                buf[i] = 0;
                addr = &buf[i + 1];

                sprintf(cmd, "addr2line -e '%s' %s 2>&1", buf, addr);
                addr2line = popen(cmd, "r");

                while (fgets(addr2lineBuf, sizeof(addr2lineBuf), addr2line) != NULL) {
                    if (strstr(addr2lineBuf, "No such file") == 0) {
                        fprintf(stderr, "trace===> %s", addr2lineBuf);
                    }
                    else {
                        fprintf(stderr, "trace---> %s", traceLine);
                    }
                }
                fclose(addr2line);
                break;
            }
        }
        if (i <= 0) {
            fprintf(stderr, "trace---> %s", traceLine);
        }
    }

    fflush(stderr);
    fclose(stream);

    fprintf(stderr, "exit stack trace");
    fflush(stderr);

    exit(1);


}

static void sigHandler(int sig) {
    FILE* stream;
    int       streamHandle;
    int       status;
    void* trace[128];
    char      tracePipe[64];
    char      buf[4046];
    size_t    size;
    pthread_t threadId;

    strcpy(tracePipe, "/tmp/stackTrace.XXXXXX");
    mktemp(tracePipe);
    sprintf(buf, "mknod %s p", tracePipe);
    system(buf);

    stream = fopen(tracePipe, "r+");
    streamHandle = fileno(stream);

    size = backtrace(trace, 128);

    fprintf(stderr, "Error: signal %d:\n", sig);

    void* args = malloc(sizeof(streamHandle));
    memcpy(args, &streamHandle, sizeof(streamHandle));
    status = pthread_create(&threadId, NULL, printStackTrace, args);

    if (status != 0) {
        printf("sigHandler::thread create failed %d--%s\n", status, strerror(errno));
        exit(9);
    }
    pthread_detach(threadId);

    // generate stack trace
    backtrace_symbols_fd(trace, size, streamHandle);
    fflush(stream);
    sprintf(buf, "rm -f %s", tracePipe);
    system(buf);

    fprintf(stream, "---end---\n");
    fflush(stream);
    fclose(stream);

    delay(1000);
    exit(2);
}


int main(int argc, char** argv) {

    signal(SIGSEGV, sigHandler);   // install our handler
    signal(SIGUSR1, sigHandler);


    char* mc = getenv("MALLOC_CHECK_");
    printf("MALLOC_CHECK_=%s\n", mc);


    if (setup() != 0) {
        fprintf(stderr, "setup failed, terminating\n");
        return 2;
    }

    printf("initialization complete\n");
    intersectionBlocked = false;


    idleTramDirections[TRAM1] = FORWARD;
    idleTramDirections[TRAM2] = FORWARD;



    while (true) {
        fflush(stdout);
        delay(1000);
    }
    return 0;
}
