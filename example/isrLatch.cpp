#include <wiringPi.h>
#include <mcp23x17rpi.h>
#include <lcd101rpi.h>
#include "Options.h"
#include "LatchOptions.h"

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command 
// which uses gpio export for setup for wiringPiSetupSys
#define	LED	17

int lcdHandle;
int lcdAddress = 0x27;



LatchOptions options = LatchOptions();

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



    mcp23x17_handle = mcp23x17_setup(false, options.i2cAddress, options.inta, options.intb);

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


int values[MCP23x17_PORTS][8];

void readAll() {
    //       1234567                1234567
    printf("+--------------++--------------+\n");
    printf("|       IIC Address 0x%02x       |\n",options.i2cAddress);
    printf("+--------------++--------------+\n");
    printf("|    Port-B    ||    Port-A    |\n");
    printf("+---+------+---++---+------+---+\n");
    printf("| n | Mode | V || V | Mode | n |\n");
    printf("+---+------+---++---+------+---+\n");

    for (int pin = 0; pin < 8; ++pin) {
        char *mode=" IN ";

        printf("| %d | %4.4s | %d || %d | %4.4s | %d |\n", 
            pin, mode, values[MCP23x17_PORTB][pin],
            
            values[MCP23x17_PORTA][7-pin] , mode, 7-pin);
    }
    printf("+---+------+---++---+------+---+\n");

    if (options.verbose) {
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

    for (int p=0;p<2;++p) {
        lcdPosition(lcdHandle,0,p);
        lcdPrintf(lcdHandle,"%c:  %d%d%d%d  %d%d%d%d",
            'A'+p,
            values[MCP23x17_PORTA+p][0],
            values[MCP23x17_PORTA+p][1],
            values[MCP23x17_PORTA+p][2],
            values[MCP23x17_PORTA+p][3],

            values[MCP23x17_PORTA+p][4],
            values[MCP23x17_PORTA+p][5],
            values[MCP23x17_PORTA+p][6],
            values[MCP23x17_PORTA+p][7]
        );
    }
}

void updateValues(MCP23x17_GPIO gpio, int value) {
    int port = mcp23x17_getPort(gpio);
    int pin  = mcp23x17_getPin(gpio);
    fprintf(stderr,"port %c pin=%d value=%d\n",'A'+port,pin,value);
    values[port][pin]=value;
}




int main(int argc, char **argv)
{
    if (!options.commandLineOptions(argc, argv)) {
        exit(EXIT_FAILURE);
    }
    if (mcp23x17_getDebug()) {
        fprintf(stderr, "inta=%d intb=%d\n", options.inta, options.intb);
    }
    if (options.inta<0  || options.intb<0) {
        fprintf(stderr, "Both interrupt pins must be supplied\n");
        return 2;
    }

    if (!setup()) {
        printf("setup failed\n");
        exit(EXIT_FAILURE);
    }

    for (int port=0 ; port < MCP23x17_PORTS; ++port) {
      for (int pin = 0; pin < 8; ++pin) {
          mcp23x17_setPinInputMode(mcp23x17_getGPIO(options.i2cAddress, port, pin), 1, updateValues);
      }
    }

    for (int pin=0;pin<8;++pin)
        for (int port = 0; port < MCP23x17_PORTS; ++port)
            values[port][pin]=1;


    while ( true ) {
      system("clear");
      readAll();
      delay(100);
    }
		    

}
