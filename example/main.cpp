/**********************************************************
 *   mcp23x17 extension library for wiringPi              *
 *                                                        *
 *   Example usage program                                *
 *                                                        *
 *   Please follow the directions to make and install     *
 *   the libaray before compliling this program.          *
 *                                                        *
 *   $ make                                               *
 *   $ ./example                                          *
 *                                                        *
 *                                                        *
 **********************************************************/
#include <mcp23x17rpi.h>
#include <sys/time.h>
#include <limits.h>
#include <unistd.h>

int txs0108_pin = 22;         // wiringpi pin number

int mcp23x17_handle   = -1;
int mcp23x17_address  = 0x20;
int mcp23x17_inta_pin = 0;    // wiringpi pin number
int mcp23x17_intb_pin = 7;    // wiringpi pin number


MCP23x17_GPIO eventPin = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 0);
MCP23x17_GPIO led1     = mcp23x17_getGPIO(mcp23x17_address, MCP23x17_PORTA, 4);


bool ledStatus = false;


unsigned long long currentTimeMillis() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    return (unsigned long long)(currentTime.tv_sec) * 1000 +
        (unsigned long long)(currentTime.tv_usec) / 1000;
}


void eventMethod(MCP23x17_GPIO gpio, int value) {

    MCP23x17_PORT port=mcp23x17_getPort(gpio);
    MCP23x17_PIN  pin=mcp23x17_getPin(gpio);

    printf("event pin<%c%d> value=%d\n", 97+port, pin, value); 

    // set LED to match event
    mcp23x17_digitalWrite(led1, value);

}




int setup() {

    if (wiringPiSetup() != 0) {
        fprintf(stderr, "Wiring Pi could not be initialized\n");
        return 9;
    }

    fprintf(stderr, "enable txs0108\n");
    pinMode(txs0108_pin, OUTPUT);
    digitalWrite(txs0108_pin, 1);    


    mcp23x17_setDebug(false);
    mcp23x17_handle = mcp23x17_setup(0, mcp23x17_address, mcp23x17_inta_pin, mcp23x17_intb_pin);

    if (mcp23x17_handle < 0) {
        fprintf(stderr, "mcp23017 could not be initialized\n");
        return 9;
    }

    if (mcp23x17_getDebug()) {
        fprintf(stderr, "mcp23x17_handle=%d\n\n", mcp23x17_handle);  
    }

    return 0;
}

int main(int argc, char** argv) {

    if (setup() != 0) {
        fprintf(stderr, "setup failed, terminating\n");
        return 2;
    }

    fprintf(stderr, "system initialization complete\n");
    fprintf(stderr, "initialize input pin(s)\n");  

    mcp23x17_setPinInputMode(eventPin, TRUE, &eventMethod);

    // read initial value and set LED to match accordingly;
    int value = mcp23x17_digitalRead(eventPin);   
    mcp23x17_digitalWrite(led1, value);

    while (true) {
        delay(500); fflush(stdout); fflush(stderr);
    }
    return 0;
}

