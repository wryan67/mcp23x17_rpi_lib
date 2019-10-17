/**********************************************************
 *   mcp23x17 extension library for wiringPi              *
 *                                                        *
 *   Example usage program                                *
 *                                                        *
 *   Please follow the directions to make and install     *
 *   the libaray before compliling this program.          *
 *
 *   $ gcc -o main -lpthread -lwiringPi -lwiringPiMCP23x17 main.cpp
 *   $ ./main
 *                                                        *
 *                                                        *
 **********************************************************/
#include <mcp23x17.h>
#include <sys/time.h>
#include <limits.h>

int mcp23x17_handle = -1;
int mcp23x17_address = 0x21;
int mcp23x17_inta_pin = 2;
int mcp23x17_intb_pin = 3;



MCP23x17_GPIO eventPin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 5);
MCP23x17_GPIO testPin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 6);
MCP23x17_GPIO led1 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 7);


bool ledStatus = false;


bool eventTracking = false;
volatile int events = 0;
volatile int transactions;
volatile int staleTimer = 0;


unsigned long long currentTimeMillis() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    return (unsigned long long)(currentTime.tv_sec) * 1000 +
        (unsigned long long)(currentTime.tv_usec) / 1000;
}




void* backgroundCounter(void*) {
    eventTracking = true;
    while (true) {
        delay(1000);
        int counter = transactions;
        transactions = 0;
        printf("epoch=%lld TPS=%4d\n", currentTimeMillis(), counter);
        if (counter == 0) {
            if (++staleTimer > 3) {
                eventTracking = false;
                pthread_exit(0);
            }
        }
        else if (staleTimer > 0) {
            staleTimer = 0;
        }
    }
}
void testMethod(int port, int pin, int value) {
    printf("test method invoked\n");
}

void counterMethod(int port, int pin, int value) {
    if ((++events % 10000) == 0) {
        printf("%d events\n", events);
        events = 0;
    }
    if (value != 0) {
        return;
    }
    if (!eventTracking) {
        pthread_t threadId;
        int status = pthread_create(&threadId, NULL, backgroundCounter, NULL);
        if (status == 0) {
            pthread_detach(threadId);
        }
        else {
            fprintf(stderr, "counterMethod::thread create failed %d--%s\n", status, strerror(errno)); fflush(stderr);
            exit(9);
        }
    }
    ++transactions;
}




int setup() {

    if (wiringPiSetup() != 0) {
        fprintf(stderr, "Wiring Pi could not be initialized\n");
        return 9;
    }

    mcp23x17_setDebug(false);
    mcp23x17_handle = mcp23x17_setup(0, mcp23x17_address, mcp23x17_inta_pin, mcp23x17_intb_pin);

    if (mcp23x17_handle < 0) {
        fprintf(stderr, "mcp23017 could not be initialized\n");
        return 9;
    }

    if (mcp23x17_getDebug()) {
        fprintf(stderr, "mcp23x17_handle=%d\n\n", mcp23x17_handle);  fflush(stderr);
    }

    fprintf(stderr, "init input pins\n");  fflush(stderr);



    mcp23x17_setPinInputMode(eventPin, TRUE, &counterMethod);
    mcp23x17_setPinInputMode(testPin, TRUE, &testMethod);


    return 0;
}

int main(int argc, char** argv) {

    if (setup() != 0) {
        fprintf(stderr, "setup failed, terminating\n");
        return 2;
    }

    printf("initialization complete\n");

    unsigned long i = (mcp23x17_getDebug()) ? 2 : ULONG_MAX;
    while (i-- > 0) {
        mcp23x17_digitalWrite(led1, HIGH);
        delay(500); fflush(stdout);
        mcp23x17_digitalWrite(led1, LOW);
        delay(500); fflush(stdout);
    }
    while (true) {
        delay(500); fflush(stdout);
    }
    return 0;
}

