/*******************************************************
 *                                                     *
 *   mcp23x17 extension library for wiringPi           *
 *   written by: Wade Ryan September 2019              *
 *                                                     *
 *   Utilizes the interrupt pins to facalitate         *
 *   writing event driven functions                    *
 *                                                     *
 *******************************************************/

#include "mcp23x17.h"
#include "mcp23x17_threads.h"

// static int mcp23x17_handle;

static volatile unsigned char portPinValues[MCP23x17_MAX_ADDRESS][MCP23x17_PORTS];

static volatile int address2handle[255] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
static volatile unsigned char gpio2address[255] = { 0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu };


static void (*isrFunctions[255][MCP23x17_PORTS][8])();

static volatile int pinModes[MCP23x17_PORTS][8];

static volatile int debug = FALSE;


int mcp23x17_getDebug() {
    return debug;
}

void mcp23x17_setDebug(int mode) {
    debug = mode;
}

int mcp23x17_getHandle(unsigned char mcp23x17_address) {
    return address2handle[mcp23x17_address];
}

void mcp23x17_setHandle(unsigned char mcp23x17_address, int handle) {
    address2handle[mcp23x17_address] = handle;
}


struct mcp23x17_eventData_struct {
    unsigned char mcp23x17_address;
    int rpiPin;
    int port;
    int pin;
    int value;
};

typedef struct mcp23x17_eventData_struct mcp23x17_eventData;

MCP23x17_GPIO mcp23x17_getGPIO(MCP23x17_ADDRESS address, MCP23x17_PORT port, MCP23x17_PIN pin) {
    return (address << 8) | ((0x01 & port) << 4) | (0x0F & pin);
}

MCP23x17_ADDRESS mcp23x17_getAddress(MCP23x17_GPIO gpio) {
    return gpio >> 8;
}

MCP23x17_PORT mcp23x17_getPort(MCP23x17_GPIO gpio) {
    return (0x0010 & gpio) >> 4;
}

MCP23x17_PIN mcp23x17_getPin(MCP23x17_GPIO gpio) {
    return 0x000F & gpio;
}


pthread_t mcp23x17_createThread(void* (*method)(void*), char* description, mcp23x17_eventData eventData) {
    pthread_t threadId;

    void* newArgs = malloc(sizeof(mcp23x17_eventData));

    if (debug) {
        fprintf(stderr, "<<mcp23x17_createThread(%s)::alloc--args %p\n", description, newArgs);
    }

    memcpy(newArgs, &eventData, sizeof(mcp23x17_eventData));

    int status = pthread_create(&threadId, NULL, method, newArgs);

    if (status != 0) {
        fprintf(stderr, "%s::thread create failed %d--%s\n", description, status, strerror(errno)); fflush(stderr);
        exit(9);
    }

    pthread_detach(threadId);
    return threadId;
}


int mcp23x17_bankAddress(int port, int address) {
    int realAddress = address & 0x0F;

    if (port == MCP23x17_PORTB) {
        realAddress |= 0x10;
    }
    return realAddress;
}

static volatile long pinExecutions = 0;

void* mcp23x17_pin_execute(void* args) {
    mcp23x17_eventData* eventData = args;

    if ((++pinExecutions % 1000)==0) {
        printf("%ld pin executions\n",pinExecutions);
    }

    if (debug) {
        fprintf(stderr, "mcp23x17_pin_execute(%02x): port=%d pin=%d value=%dx\n", eventData->mcp23x17_address, eventData->port, eventData->pin, eventData->value); fflush(stderr);
    }

    isrFunctions[eventData->mcp23x17_address][eventData->port][eventData->pin](eventData->port, eventData->pin, eventData->value);

    if (debug) {
        fprintf(stderr, "<<mcp23x17_pin_execute::free--args %p\n", args); fflush(stderr);
    }

    free(args);
    pthread_exit(NULL);
}


void* mcp23x17_intx_execute(void* args) {
    char  description[64];

    // known at this point are rpiPin and mcp23x17 port (a or b)

    mcp23x17_eventData* eventData = args;


    unsigned char mcp23x17_address = gpio2address[eventData->rpiPin];
    int mcp23x17_handle = address2handle[mcp23x17_address];

    unsigned char _portValues = portPinValues[mcp23x17_address][eventData->port];
    unsigned char _newValues;
    unsigned char registerAddress = MCP23x17_INTCAP(eventData->port);

    if (debug) {
        fprintf(stderr, "mcp23x17_intx_execute(%02x): port=%c register=%02x\n", mcp23x17_address, 65+eventData->port, registerAddress); fflush(stderr);
        fprintf(stderr, "mcp23x17_intx_execute(%02x): before pinValuesPort[0]=%02x\n", mcp23x17_address, portPinValues[mcp23x17_address][0]);
        fprintf(stderr, "mcp23x17_intx_execute(%02x): before pinValuesPort[1]=%02x\n", mcp23x17_address, portPinValues[mcp23x17_address][1]); fflush(stderr);
    }

    _newValues = wiringPiI2CReadReg8(mcp23x17_handle, registerAddress);
    portPinValues[gpio2address[eventData->rpiPin]][eventData->port] = _newValues;

    if (debug) {
        fprintf(stderr, "mcp23x17_intx_execute(%02x): after  pinValuesPort[0]=%02x\n", mcp23x17_address, portPinValues[mcp23x17_address][0]);
        fprintf(stderr, "mcp23x17_intx_execute(%02x): after  pinValuesPort[1]=%02x\n", mcp23x17_address, portPinValues[mcp23x17_address][1]); fflush(stderr);
    }

    for (int i = 0; i < 8; ++i) {
        int x1 = _portValues & 0x01;
        int x2 = _newValues & 0x01;

        _portValues = _portValues >> 1;
        _newValues = _newValues   >> 1;

        if (x1 != x2 && pinModes[eventData->port][i] == INPUT) {
            sprintf(description, "mcp23x17_int%c_pin_%d_execute", 97 + eventData->port, x2);

            eventData->pin   = i;
            eventData->value = x2;
            eventData->mcp23x17_address = mcp23x17_address;

            mcp23x17_createThread(mcp23x17_pin_execute, description, *eventData);

        }
    }

    if (debug) {
        fprintf(stderr, "mcp23x17_intx_execute::free %p\n", args);
    }

    free(args);
    pthread_exit(NULL);
}


void mcp23x17_intx_activated(int rpiPin, MCP23x17_PORT port) {
    char description[64];
    sprintf(description, "mcp23x17_int%c_activated rpiPin=%d", 97+port, rpiPin);

    mcp23x17_eventData eventData;
    eventData.rpiPin = rpiPin;
    eventData.port   = port;

    if (debug) {
        fprintf(stderr, "%s; port-%c\n", description, eventData.port); fflush(stderr);
    }
    mcp23x17_createThread(mcp23x17_intx_execute, description, eventData);
}


void mcp23x17_inta_activated(int pin) {
    mcp23x17_intx_activated(pin, MCP23x17_PORTA);
}

void mcp23x17_intb_activated(int pin) {
    mcp23x17_intx_activated(pin, MCP23x17_PORTB);
}

char* right(char* s, int pos) {
    return &s[(strlen(s) - pos)];
}

void mcp23x17_updateRegister(int registerAddress, MCP23x17_GPIO gpio, int value) {
    int mcp23x17_address = mcp23x17_getAddress(gpio);
    int port = mcp23x17_getPort(gpio);
    int pin = mcp23x17_getPin(gpio);
    int mcp23x17_handle = address2handle[mcp23x17_address];

    unsigned char newValue;
    unsigned char oldValue;
    int adress = mcp23x17_bankAddress(port, registerAddress);

    if (debug) {
        fprintf(stderr, "mcp23x17_updateRegister mcpHandle=%d, mcpAddress=%02x, port=%c, pin=%d, value=%d\n", mcp23x17_handle, mcp23x17_address, 65 + port, pin, value);
    }
    
    oldValue = wiringPiI2CReadReg8(mcp23x17_handle, adress);
    unsigned char pinIndex = 1 << pin;

    if (value == 1) {
        newValue = oldValue | pinIndex;
    } else {
        newValue = oldValue & ~pinIndex;
    }
    
    if (debug) {
        char strPinIndex[12];
        char strPinIndexComplement[32];
        sprintf(strPinIndex, "%02x", pinIndex);
        sprintf(strPinIndexComplement, "%02x", ~pinIndex);

        fprintf(stderr, "mcp23x17_updateRegister port=%c, pin=%d, register=%02x oldValue=%02x, newValue=%02x  pinIndex=%2.2s ~pinIndex=%s\n", 
            65+port, pin, adress, oldValue, newValue, right(strPinIndex,2), right(strPinIndexComplement,2));
        fflush(stderr);
    }
    wiringPiI2CWriteReg8(mcp23x17_handle, adress, newValue);
}


int mcp23x17_setup(int spi, MCP23x17_ADDRESS mcp23x17_address, int mcp23x17_inta_pin, int mcp23x17_intb_pin) {
    unsigned char c;
    int mcp23x17_handle;

    if (spi != 0) {
        fprintf(stderr, "SPI mode is not implemented yet\n"); fflush(stderr);
        return -1;
    }

    pinMode(mcp23x17_intb_pin, INPUT);
    pullUpDnControl(mcp23x17_intb_pin, PUD_UP);


    mcp23x17_openHandle(mcp23x17_address);
    if ((mcp23x17_handle=address2handle[mcp23x17_address]) < 0) {
        return -2;
    }


    if (mcp23x17_inta_pin >= 0) {
        pinMode(mcp23x17_inta_pin, INPUT);
        pullUpDnControl(mcp23x17_inta_pin, PUD_UP);

        if (gpio2address[mcp23x17_inta_pin] != 0xFFu) {
            fprintf(stderr, "gpio pin %d (inta) is already associated with a different address, namely %02x\n",
                mcp23x17_inta_pin, gpio2address[mcp23x17_inta_pin]); fflush(stderr);
            return -3;
        }
        gpio2address[mcp23x17_inta_pin] = mcp23x17_address;
    }

    if (mcp23x17_intb_pin >= 0 && mcp23x17_inta_pin != mcp23x17_intb_pin) {
        if (gpio2address[mcp23x17_intb_pin] != 0xFFu) {
            fprintf(stderr, "gpio pin %d (intb) is already associated with a different address, namely %02x\n",
                mcp23x17_intb_pin, gpio2address[mcp23x17_intb_pin]); fflush(stderr);
            return -4;
        }
        gpio2address[mcp23x17_intb_pin] = mcp23x17_address;
    }


    if (debug) {
        fprintf(stderr, "args:    mcp23x17_handle=%d mcp23x17_address=%02x mcp23x17_inta_pin=%d mcp23x17_intb_pin=%d\n", 
                                  mcp23x17_handle,   mcp23x17_address,     mcp23x17_inta_pin,   mcp23x17_intb_pin);
        fprintf(stderr, "setup-a: mcp23x17_handle=%d mcp23x17_address=%02x \n", 
                                  address2handle[gpio2address[mcp23x17_inta_pin]],  gpio2address[mcp23x17_inta_pin] );
        fprintf(stderr, "setup-b: mcp23x17_handle=%d mcp23x17_address=%02x \n",
                                  address2handle[gpio2address[mcp23x17_intb_pin]], gpio2address[mcp23x17_intb_pin]);
    }


/*   --Hardware Reset--
//  Note that I tried using the brown wire in the cat5 cabel for 
//  power-on-reset (POR), but due to cross contamination on the 
//  brown/brown-white wires, POR does not work.  Well, technically 
//  it does work, but when INT-B is triggerd, so is POR, and thus
//  everhting is reset and your code stops working as it's now
//  out of sync with the deivce.  DOH!
//  --INT-B is on the brown-white wire.

    pinMode(mcp23x17_por_pin,  OUTPUT);  

    digitalWrite(mcp23x17_por_pin, 0);
    delay(5);  // Mandatory reset delay
    digitalWrite(mcp23x17_por_pin, 1);
*/

/*   --Software Reset--
 *   Note that this could overwrite GPINTENB.GPINT7
 */
    // Set iocon.bank=0 -or- disable GPINTENB.GPINT7 
    c = wiringPiI2CReadReg8(mcp23x17_handle, 0x05);
    wiringPiI2CWriteReg8(mcp23x17_handle, 0x05, c & 0x7f);
    delay(5);

    // set iocon.bank=1  (8-bit mode)
    c = wiringPiI2CReadReg8(mcp23x17_handle, 0x0a); 
    wiringPiI2CWriteReg8(mcp23x17_handle, 0x0a, c | 0x80);
    delay(5);

    // set non-seq; 8-bit config        (IOCON)
    wiringPiI2CWriteReg8(mcp23x17_handle, 0x05, 0xA8);
    delay(5);

    c = wiringPiI2CReadReg8(mcp23x17_handle, 0x05);
    if (c != 0xA8) {
        fprintf(stderr, "Unable to configure mcp23x17 at address: 0x%02x\n", mcp23x17_address); fflush(stderr);
        return -1;
    }

    // almost factory defaults
    for (int port = 0; port < MCP23x17_PORTS; ++port) {
        wiringPiI2CWriteReg8(mcp23x17_handle, MCP23x17_OLAT(port),    0x00);
        wiringPiI2CWriteReg8(mcp23x17_handle, MCP23x17_IODIR(port),   0x00);
        wiringPiI2CWriteReg8(mcp23x17_handle, MCP23x17_IPOL(port),    0x00);
        wiringPiI2CWriteReg8(mcp23x17_handle, MCP23x17_GPINTEN(port), 0x00);
        wiringPiI2CWriteReg8(mcp23x17_handle, MCP23x17_DEFVAL(port),  0x00);
        wiringPiI2CWriteReg8(mcp23x17_handle, MCP23x17_INTCON(port),  0x00);
        wiringPiI2CWriteReg8(mcp23x17_handle, MCP23x17_GPPU(port),    0x00);
        wiringPiI2CWriteReg8(mcp23x17_handle, MCP23x17_INTF(port),    0x00);
        wiringPiI2CWriteReg8(mcp23x17_handle, MCP23x17_INTCAP(port),  0x00);
    }
    

    // initialize all pins for output mode, with low values and pull-up resistor on
    for (int port=0;port<MCP23x17_PORTS;++port) {
        for (int pin=0;pin<8;++pin) {
            mcp23x17_setPinOutputMode(mcp23x17_getGPIO(mcp23x17_address, port, pin), LOW);
        }
    }

    // clear interrupts && get initial values;
    portPinValues[mcp23x17_address][0] = wiringPiI2CReadReg8(mcp23x17_handle, MCP23x17_GPIO(MCP23x17_PORTA));
    portPinValues[mcp23x17_address][1] = wiringPiI2CReadReg8(mcp23x17_handle, MCP23x17_GPIO(MCP23x17_PORTB));

    // setup interrupt pin event triggers
    if (mcp23x17_inta_pin > 0 && mcp23x17_wiringPiISRWithPin(mcp23x17_inta_pin, INT_EDGE_FALLING, &mcp23x17_inta_activated) < 0)
    {
        fprintf(stderr, "Unable to setup ISR on pin %d: %s\n", mcp23x17_inta_pin, strerror(errno)); fflush(stderr);
        return -1;
    }

    if (mcp23x17_inta_pin > 0 && mcp23x17_wiringPiISRWithPin(mcp23x17_intb_pin, INT_EDGE_FALLING, &mcp23x17_intb_activated) < 0)
    {
        fprintf(stderr, "Unable to setup ISR on pin %d: %s\n", mcp23x17_intb_pin, strerror(errno)); fflush(stderr);
        return -1;
    }

    if (debug) {
        fprintf(stderr, "portPinValues[%02x][0]=%02x\n", mcp23x17_address, portPinValues[mcp23x17_address][0]);
        fprintf(stderr, "portPinValues[%02x][1]=%02x\n", mcp23x17_address, portPinValues[mcp23x17_address][1]); fflush(stderr);
    }

    return mcp23x17_handle;
}



void mcp23x17_setPinInputMode(MCP23x17_GPIO gpio, int enablePullUp, void (*function)(int port, int pin, int value)) {
    int address = mcp23x17_getAddress(gpio);
    int port    = mcp23x17_getPort(gpio);
    int pin     = mcp23x17_getPin(gpio);

    int saveModes[8];
    for (int i = 0; i < 8; ++i) {
        saveModes[i] = pinModes[port][i];
        pinModes[port][i] = OUTPUT;
    }

    mcp23x17_updateRegister(0x00, gpio, 1);                // output mode
    mcp23x17_updateRegister(0x0A, gpio, enablePullUp);     // initial value
    mcp23x17_updateRegister(0x00, gpio, 1);                // IODIR     I/O Direction        0=output;    1=input
    mcp23x17_updateRegister(0x01, gpio, 0);                // IPOL      I/O Polarity         0=normal;    1=inverted
    mcp23x17_updateRegister(0x06, gpio, enablePullUp);     // GPPU      Pull-Up Resistor     0=disabled;  1=enabled
    mcp23x17_updateRegister(0x03, gpio, 1);                // DEFVAL    Compare IOC          0=Rising;    1=falling
    mcp23x17_updateRegister(0x04, gpio, 0);                // INTCON    Interrupt Control    0=AnyChange; 1=DEFVAL

    portPinValues[address][port] = wiringPiI2CReadReg8(address2handle[address], mcp23x17_bankAddress(port, 0x09));   // GPIO

    mcp23x17_updateRegister(0x02, gpio, 1);                // GPINTEN   Interupt On Change   0=disabled;  1=enabled

    saveModes[pin] = INPUT;
    isrFunctions[address][port][pin] = function;

    for (int i = 0; i < 8; ++i) {
        pinModes[port][i] = saveModes[i];
    }

    if (debug) {
        fprintf(stderr, "mcp23x17_setPinInputMode(%02x)::set port %d, pin %d as input\n", address, port, pin);
        fprintf(stderr, "mcp23x17_setPinInputMode(%02x)::portPinValues[%02x][%d]=%02x\n", address, address, port, portPinValues[address][port]); fflush(stderr);
    }
}


void mcp23x17_setPinOutputMode(MCP23x17_GPIO gpio, int initialValue) {
    int port = mcp23x17_getPort(gpio);
    int pin  = mcp23x17_getPin(gpio);

    pinModes[port][pin]=OUTPUT;

    if (debug) {
        fprintf(stderr, "mcp23x17_setPinOutputMode port=%c, pin=%d initialValue=%d\n", 65 + port, pin, initialValue);
    }
    mcp23x17_updateRegister(0x00, gpio, 0);                // IODIR     I/O Direction        0=output;    1=input
    mcp23x17_updateRegister(0x0A, gpio, initialValue);     // OLAT
    mcp23x17_setVirtualPinValue(gpio,initialValue);
}



void mcp23x17_openHandle(MCP23x17_ADDRESS address) {
    if (address2handle[address] < 0) {
        address2handle[address] = wiringPiI2CSetup(address);
        if (address2handle[address] < 0) {
            fprintf(stderr, "Unable to open handle for mcp23x17 at address 0x%02x: %s\n", address, strerror(errno)); fflush(stderr);
        }
    } 
}

void mcp23x17_digitalWrite(MCP23x17_GPIO gpio, int value) {
    mcp23x17_openHandle(mcp23x17_getAddress(gpio));
    mcp23x17_updateRegister(0x0A, gpio, value);
    mcp23x17_setVirtualPinValue(gpio, value);
}

int  mcp23x17_digitalRead(MCP23x17_GPIO gpio) {
    mcp23x17_openHandle(mcp23x17_getAddress(gpio));

    unsigned char regValue = wiringPiI2CReadReg8(address2handle[mcp23x17_getAddress(gpio)], MCP23x17_GPIO(mcp23x17_getPort(gpio)));

    return 0x01 & (regValue >> mcp23x17_getPin(gpio));
}


unsigned char mcp23x17_virtualReadPort(MCP23x17_ADDRESS address, MCP23x17_PORT port) {
    return portPinValues[address][port];
}

int mcp23x17_virtualRead(MCP23x17_GPIO gpio) {
    unsigned char pinValues = portPinValues[mcp23x17_getAddress(gpio)][mcp23x17_getPort(gpio)] >> mcp23x17_getPin(gpio);

    return pinValues & 0x01;
}



void mcp23x17_setVirtualPinValue(MCP23x17_GPIO gpio, int value) {
    unsigned char pinIndex = 1 << mcp23x17_getPin(gpio);

    if (value == 1) {
        portPinValues[mcp23x17_getAddress(gpio)][mcp23x17_getPort(gpio)] |= pinIndex;
    } else {
        portPinValues[mcp23x17_getAddress(gpio)][mcp23x17_getPort(gpio)] &= ~pinIndex;
    }
}
