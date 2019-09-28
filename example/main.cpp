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
 *   This program assumes you have an LED plugged         *
 *   into port-B on pin 0 and 1 or more sensors           *
 *   plugged into Port-A on GPIO pins 0,1                 *
 *   or plugged into port-B on GPIO pins 6,7              *
 *   Where your device operates on an open collecotor     *
 *   principal where the output is drivin high using      *
 *   a pull-up resistor and when the device trips,        *
 *   it drives the voltage to ground.  I'm using          *
 *   a3144 hall sensors to test with.                     *
 *                                                        *
 *   Note that you will need a resistor on your LED,      *
 *   but probably not on your sensors.  The mcp23x17      *
 *   has internal pull-up resistors that can be used      *
 *   for input pins, and this program does use them.      *
 *                                                        *
 *   Another option would be to simply place a            *
 *   normally open button between one of the              *
 *   input pins and ground.                               *
 *                                                        *
 *   Note that placing a sensor or button on port-A,      *
 *   pin #3, will track transactions per second, where    *
 *   a transaction is considered to be the trigger event  *
 *   only, so the counter is on the low signal only.      *
 *   But your device will actually be generating an       *
 *   interrupt for both the low and high states,          *
 *   so twice the TPS shown by the counter.               *
 *                                                        *
 *   If my math is correct, when operating at 100k htz,   *
 *   the MCP23017 can generate arount 2777 interrupts     *
 *   per second, or about 1388 TPS.  In practice though,  *
 *   I'm seeing consistent results up to 330 TPS          *
 *   (660 events/interrupts) per second.  Abve that       *
 *   frequency and events start getting lost.   So,       *
 *   in practice, I'm seeing about half of what the       *
 *   expected maximum events should be.  I'm not sure     *
 *   what the reason is yet.                              *
 *
 *                                                        *
 **********************************************************/
#include "../src/mcp23x17.h"
#include <sys/time.h>

int mcp23x17_handle = -1;
int mcp23x17_address = 0x20;
int mcp23x17_inta_pin = 27;
int mcp23x17_intb_pin = 28;



MCP23x17_GPIO LED1 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 0);
MCP23x17_GPIO LED2 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 7);

MCP23x17_GPIO northPin_0 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 0);
MCP23x17_GPIO northPin_1 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 1);

MCP23x17_GPIO southPin_6 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 6);
MCP23x17_GPIO southPin_7 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 7);

MCP23x17_GPIO eventPin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 6);


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




void toggleLED() {
    ledStatus = !ledStatus;
    mcp23x17_digitalWrite(LED1, ledStatus);
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

void counterMethod(int port, int pin, int value) {
    if ((++events % 10000) == 0) {
        printf("%d events\n",events);
        events = 0;
    }
    if (value != 0) {
        return;
    }
    if (!eventTracking) {
        pthread_t threadId;
        int status=pthread_create(&threadId, NULL, backgroundCounter, NULL);
        if (status == 0) {
            pthread_detach(threadId);
        } else {
            fprintf(stderr, "counterMethod::thread create failed %d--%s\n", status, strerror(errno)); fflush(stderr);
            exit(9);
        }
    }
    ++transactions;
}


void southGate(int port, int pin, int value) {
    toggleLED();
	printf("south gate tripped: port-%c, pin-%d, value-%d; occured at %llu ms\n", 65+port, pin, value, currentTimeMillis()); 	fflush(stdout);
}


void northGate(int port, int pin, int value) {
    toggleLED();
    printf("north gate tripped: port-%c, pin-%d, value-%d; occured at %llu ms\n", 65+port, pin, value, currentTimeMillis()); 	fflush(stdout);
}


int setup() {

	if (wiringPiSetup() != 0) {
		fprintf(stderr, "Wiring Pi could not be initialized\n");
		return 9;
	}

    mcp23x17_setDebug(false);
    mcp23x17_handle = mcp23x17_setup(0, mcp23x17_address, mcp23x17_inta_pin, mcp23x17_intb_pin);

    if (mcp23x17_handle<0) {
		fprintf(stderr, "mcp23017 could not be initialized\n");
		return 9;
	}

    if (mcp23x17_getDebug()) {
        fprintf(stderr, "mcp23x17_handle=%d\n\n", mcp23x17_handle);  fflush(stderr);
    }

    fprintf(stderr, "init input pins\n");  fflush(stderr);


    if (mcp23x17_getDebug()) {
        fprintf(stderr, "northPin_0=%04x\n", northPin_0);
        fprintf(stderr, "northPin_1=%04x\n", northPin_1);
    }

    mcp23x17_setPinInputMode(northPin_0, TRUE, &northGate);
    mcp23x17_setPinInputMode(northPin_1, TRUE, &northGate);

    mcp23x17_setPinInputMode(southPin_6, TRUE, &southGate);
    mcp23x17_setPinInputMode(southPin_7, TRUE, &southGate);


    mcp23x17_setPinInputMode(eventPin, TRUE, &counterMethod);

    fprintf(stderr, "init output pin\n");  fflush(stderr);

    mcp23x17_setPinOutputMode(LED1, LOW);
    mcp23x17_setPinOutputMode(LED2, LOW);

    return 0;
}

int main(int argc, char **argv) {

	if (setup() != 0) {
		fprintf(stderr,"setup failed, terminating\n");
		return 2;
	}

    if (mcp23x17_getDebug()) {
        unsigned char valuesPortA = mcp23x17_virtualReadPort(mcp23x17_address, MCP23x17_PORTA);
        unsigned char valuesPortB = mcp23x17_virtualReadPort(mcp23x17_address, MCP23x17_PORTB);

        fprintf(stderr, "initial virtual values portA=%02x\n", valuesPortA);
        fprintf(stderr, "initial virtual values portB=%02x\n", valuesPortB);

        valuesPortA = wiringPiI2CReadReg8(mcp23x17_handle, MCP23x17_GPIO(MCP23x17_PORTA));
        valuesPortB = wiringPiI2CReadReg8(mcp23x17_handle, MCP23x17_GPIO(MCP23x17_PORTB));

        fprintf(stderr, "initial  actual values portA=%02x\n", valuesPortA);
        fprintf(stderr, "initial  actual values portB=%02x\n", valuesPortB);

        valuesPortA = wiringPiI2CReadReg8(mcp23x17_handle, MCP23x17_INTCAP(MCP23x17_PORTA));
        valuesPortB = wiringPiI2CReadReg8(mcp23x17_handle, MCP23x17_INTCAP(MCP23x17_PORTB));

        fprintf(stderr, "initial  INTCAP values portA=%02x\n", valuesPortA);
        fprintf(stderr, "initial  INTCAP values portB=%02x\n", valuesPortB);


    }

	printf("initialization complete\n");
	

	while (true) {
        mcp23x17_setPinOutputMode(LED2, HIGH);
        delay(1000); fflush(stdout);

        mcp23x17_setPinOutputMode(LED2, LOW);
        delay(1000); fflush(stdout);
    }
	return 0;
}
