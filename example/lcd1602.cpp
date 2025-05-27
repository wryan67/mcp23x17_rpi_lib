#include <wiringPi.h>
#include <mcp23x17rpi.h>
#include <lcd101rpi.h>
#include "Options.h"

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command 
// which uses gpio export for setup for wiringPiSetupSys
#define	LED	17

int lcdHandle;
int lcdAddress = 0x27;

Options options = Options();

int mcp23x17_handle = -1;

int mcp23x17_softSetup(int spi, int mcp23x17_address) {
    if (spi != 0) {
        fprintf(stderr, "SPI mode is not implemented yet\n"); fflush(stderr);
        return -2;
    }

    if ((mcp23x17_handle = wiringPiI2CSetup(mcp23x17_address)) < 0) {
        fprintf(stderr, "Unable to setup mcp23x17: %s\n", strerror(errno)); fflush(stderr);
        return -1;
    }

    mcp23x17_setHandle(mcp23x17_address, mcp23x17_handle);
    return mcp23x17_handle;
}


bool setup() {

    if (int ret = wiringPiSetup()) {
        fprintf(stderr, "Wiring Pi setup failed, ret=%d\n", ret);
        exit(EXIT_FAILURE);
    }

    if (options.reset) {
        mcp23x17_handle = mcp23x17_setup(false, options.i2cAddress, -1, -1);
    } else {
        mcp23x17_handle = mcp23x17_softSetup(0, options.i2cAddress);
    }
    if (mcp23x17_handle < 0) {
        fprintf(stderr, "mcp23017 could not be initialized\n");
        exit(EXIT_FAILURE);
    }

    lcdHandle=lcdSetup(lcdAddress);
    if (lcdHandle < 0) {  
        fprintf(stderr, "lcdInit failed\n");  
        return 2; 
    }
     

    return true;
} 

void write2pin() {
    if (options.pin < 0) {
        fprintf(stderr, "write mode requested, but no pin specified\n");
        exit(EXIT_FAILURE);
    }

    if (options.port < 0) {
        fprintf(stderr, "write mode requested, but no port specified\n");
        exit(EXIT_FAILURE);
    }

    if (options.value < 0) {
        fprintf(stderr, "write mode requested, but no value specified\n");
        exit(EXIT_FAILURE);
    }

    if (options.debug) {
        fprintf(stderr, "setting mcp23x17 address 0x%02x port-%c pin-%d value-%d\n", options.i2cAddress, 65+options.port, options.pin, options.value);
    }
    mcp23x17_setPinOutputMode(mcp23x17_getGPIO(options.i2cAddress, options.port, options.pin), options.value);
}

void readAll() {
    unsigned char iodirValues[MCP23x17_PORTS];
    unsigned char olatValues[MCP23x17_PORTS];
    unsigned char gpioValues[MCP23x17_PORTS];

    

    int values[MCP23x17_PORTS][8];
    int modes[MCP23x17_PORTS][8];

    for (int port = 0; port < MCP23x17_PORTS; ++port) {
        int regIODIR = iodirValues[port]= wiringPiI2CReadReg8(mcp23x17_handle, MCP23x17_IODIR(port));
        int regOLAT  = olatValues[port] = wiringPiI2CReadReg8(mcp23x17_handle, MCP23x17_OLAT(port));
        int regGPIO  = gpioValues[port] = wiringPiI2CReadReg8(mcp23x17_handle, MCP23x17_GPIO(port));

        iodirValues[port]= regIODIR;
        olatValues[port] = regOLAT; 
        gpioValues[port] = regGPIO;

        char binIODIR[10];
        char binOLAT[10];
        char binGPIO[10];
  
        memset(binIODIR,   0, 10);
        memset(binOLAT,    0, 10);
        memset(binGPIO,    0, 10);
        memset(binIODIR, ' ',  9);
        memset(binOLAT,  ' ',  9);
        memset(binGPIO,  ' ',  9);
    }

    for (int port = 0; port < MCP23x17_PORTS; ++port) {
        for (int pin = 0; pin < 8; ++pin) {
            int iodir = iodirValues[port] & 0x01;
            int olat = olatValues[port] & 0x01;
            int gpio = gpioValues[port] & 0x01;

            modes[port][pin] = iodir;

            if (iodir==1) { // 1==input;  0=output
                values[port][pin] = gpio;
            } else {
                values[port][pin] = olat;
            }
            
            iodirValues[port] = iodirValues[port] >> 1;
            olatValues[port]  = olatValues[port]  >> 1;
            gpioValues[port]  = gpioValues[port]  >> 1;
        }
    }

    lcdPosition(lcdHandle,0,0);
    lcdPrintf(lcdHandle,"A: %d%d %d%d  %d%d %d%d",
            values[MCP23x17_PORTA][0],
            values[MCP23x17_PORTA][1],
            values[MCP23x17_PORTA][2],
            values[MCP23x17_PORTA][3],

            values[MCP23x17_PORTA][4],
            values[MCP23x17_PORTA][5],
            values[MCP23x17_PORTA][6],
            values[MCP23x17_PORTA][7]
        );

    lcdPosition(lcdHandle,0,1);
    lcdPrintf(lcdHandle,"B: %d%d %d%d  %d%d %d%d",
            values[MCP23x17_PORTB][0],
            values[MCP23x17_PORTB][1],
            values[MCP23x17_PORTB][2],
            values[MCP23x17_PORTB][3],

            values[MCP23x17_PORTB][4],
            values[MCP23x17_PORTB][5],
            values[MCP23x17_PORTB][6],
            values[MCP23x17_PORTB][7]
        );

    
    //       1234567                1234567
    printf("+--------------++--------------+\n");
    printf("|       IIC Address 0x%02x       |\n",options.i2cAddress);
    printf("+--------------++--------------+\n");
    printf("|    Port-B    ||    Port-A    |\n");
    printf("+---+------+---++---+------+---+\n");
    printf("| n | Mode | V || V | Mode | n |\n");
    printf("+---+------+---++---+------+---+\n");

    for (int pin = 0; pin < 8; ++pin) {
        char* mode_a = (char*)((modes[MCP23x17_PORTA][7-pin] == 0) ? " OUT" : " IN ");
        char* mode_b = (char*)((modes[MCP23x17_PORTB][  pin] == 0) ? " OUT" : " IN ");
        
        printf("| %d | %4.4s | %d || %d | %4.4s | %d |\n", 
            pin, mode_b, values[MCP23x17_PORTB][pin],
            
            values[MCP23x17_PORTA][7-pin] , mode_a, 7-pin);
    }
    printf("+---+------+---++---+------+---+\n");

    if (options.verbose) {

//  printf("+---+------+---++---+------+---+\n");
    printf("|  spi/i2c |Phy||Phy| Label    |\n");
    printf("+----------+---++---+----------+\n");
    printf("|     Vdd  | 9 || 20| Int-A    |\n");
    printf("|     Vss  |10 || 19| Int-B    |\n");
    printf("|   CS/NC  |11 || 18| Reset    |\n");
    printf("| SCK/SCL  |12 || 17| A2       |\n");
    printf("|  SI/SDA  |13 || 16| A1       |\n");
    printf("|   SO/NC  |14 || 15| A0       |\n");
    printf("+----------+---++---+----------+\n");


    }

    fflush(stdout);
}

void doNothing(MCP23x17_GPIO gpio, int value) {
  return;
}

void setMode() {
   switch (options.mode) {
   case 'o':
       mcp23x17_setPinOutputMode(mcp23x17_getGPIO(options.i2cAddress, options.port, options.pin), options.value);
       break;

   case 'i':
       if (options.value < 0) {
           options.value = 1;
       }
       mcp23x17_setPinInputMode(mcp23x17_getGPIO(options.i2cAddress, options.port, options.pin), options.value, doNothing);
       break;
   }  
   return;
}

int main(int argc, char **argv)
{
    if (!options.commandLineOptions(argc, argv)) {
        exit(EXIT_FAILURE);
    }

    if (!setup()) {
        printf("setup failed\n");
        exit(EXIT_FAILURE);
    }

    options.mode  = 'i';
    options.value = 1;
    for (options.port=0 ; options.port < MCP23x17_PORTS; ++options.port) {
      for (options.pin = 0; options.pin < 8; ++options.pin) {
        setMode();
      }
    }


    while ( true ) {
      system("clear");
      readAll();
      delay(100);
    }
		    

}
