/**********************************************************
 *   mcp23x17 extension library for wiringPi              *
 *                                                        *
 *   Example usage program                                *
 *                                                        *
 *   Please follow the directions to make and install     *
 *   the libaray before compliling this program.          *
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
 *   $ gcc -o main -lwiringPi -lwiringPiMCP23x17 main.cpp *
 *   $ ./main                                             *
 **********************************************************/
#include "../src/mcp23x17.h"
#include <sys/time.h>

int mcp23x17_handle   = -1;
int mcp23x17_address  = 0x20;
int mcp23x17_inta_pin = 27;
int mcp23x17_intb_pin = 28;


MCP23x17_GPIO LED1;  // PORT-B; pin-0
MCP23x17_GPIO LED2;  // PORT-A; pin-7

#define led1Pin 0 
#define led2Pin 7


bool ledStatus = false;

unsigned long long currentTimeMillis() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    return (unsigned long long)(currentTime.tv_sec)  * 1000 +
           (unsigned long long)(currentTime.tv_usec) / 1000;
}


void toggleLED() {
    ledStatus = !ledStatus;
    mcp23x17_digitalWrite(LED1, ledStatus);
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

    mcp23x17_setDebug(true);
    mcp23x17_handle = mcp23x17_setup(0, mcp23x17_address, mcp23x17_inta_pin, mcp23x17_intb_pin);

    if (mcp23x17_handle<0) {
		fprintf(stderr, "mcp23017 could not be initialized\n");
		return 9;
	}

    if (mcp23x17_getDebug()) {
        fprintf(stderr, "mcp23x17_handle=%d\n\n", mcp23x17_handle);  fflush(stderr);
    }

    fprintf(stderr, "init input pins\n");  fflush(stderr);

    MCP23x17_GPIO northPin_0 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 0);
    MCP23x17_GPIO northPin_1 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 1);

    MCP23x17_GPIO southPin_6 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 6);
    MCP23x17_GPIO southPin_7 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, 7);

    if (mcp23x17_getDebug()) {
        fprintf(stderr, "northPin_0=%04x\n", northPin_0);
        fprintf(stderr, "northPin_1=%04x\n", northPin_1);
    }

    mcp23x17_setPinInputMode(northPin_0, TRUE, &northGate);
    mcp23x17_setPinInputMode(northPin_1, TRUE, &northGate);

    mcp23x17_setPinInputMode(southPin_6, TRUE, &southGate);
    mcp23x17_setPinInputMode(southPin_7, TRUE, &southGate);

    fprintf(stderr, "init output pin\n");  fflush(stderr);

    LED1 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTB, led1Pin);
    LED2 = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, led2Pin);
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
//        mcp23x17_setPinOutputMode(LED2, HIGH);
//        delay(1000); fflush(stdout);

//        mcp23x17_setPinOutputMode(LED2, LOW);
        delay(1000); fflush(stdout);
    }
	return 0;
}
