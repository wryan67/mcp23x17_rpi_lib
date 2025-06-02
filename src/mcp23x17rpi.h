#pragma once
/*******************************************************
 *                                                     *
 *   mcp23x17 extension library for wiringPi           *
 *   written by: Wade Ryan September 2019              *
 *                                                     *
 *******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#ifndef NULL
#define NULL 0
#endif

#define MCP23x17_PORTS 2
#define MCP23x17_PORTA 0
#define MCP23x17_PORTB 1

#define MCP23x17_MAX_ADDRESS 0xff

#define MCP23x17_IODIR(port)    mcp23x17_bankAddress(port, 0x00)  // I/O Direction
#define MCP23x17_IPOL(port)     mcp23x17_bankAddress(port, 0x01)  // Input Polarity 
#define MCP23x17_GPINTEN(port)  mcp23x17_bankAddress(port, 0x02)  // Enable Interrupt-On-Change (IOC)
#define MCP23x17_DEFVAL(port)   mcp23x17_bankAddress(port, 0x03)  // IOC Default Compare Values
#define MCP23x17_INTCON(port)   mcp23x17_bankAddress(port, 0x04)  // Interrupt Control; Use DEFVAL or Previous
#define MCP23x17_IOCON(port)    mcp23x17_bankAddress(port, 0x05)  // Global Configruation
#define MCP23x17_GPPU(port)     mcp23x17_bankAddress(port, 0x06)  // Input Pull-Up Resistor; 100k Ohms
#define MCP23x17_INTF(port)     mcp23x17_bankAddress(port, 0x07)  // Interrupt flag (who done it)
#define MCP23x17_INTCAP(port)   mcp23x17_bankAddress(port, 0x08)  // Last interrupt captured values; clears interrupt
#define MCP23x17_GPIO(port)     mcp23x17_bankAddress(port, 0x09)  // Current GPIO values; also clears interrupt
#define MCP23x17_OLAT(port)     mcp23x17_bankAddress(port, 0x0a)  // Output Latch 


#ifdef __cplusplus
extern "C"
{
#endif  
    extern int mcp23x17_inta_pin;
    extern int mcp23x17_intb_pin;

    typedef unsigned char MCP23x17_ADDRESS;  // 0x00 - 0xFF
    typedef unsigned char MCP23x17_PORT;     // 0 or 1
    typedef unsigned char MCP23x17_PIN;      // 0-7
    typedef unsigned int  MCP23x17_GPIO;     // (ADDRESS << 8) | ( (0x01 & PORT) << 4 ) | (0x0F & PIN)

    int  mcp23x17_getDebug();
    void mcp23x17_setDebug(int mode);

    int  mcp23x17_getHandle(unsigned char mcp23x17_address);
    void mcp23x17_setHandle(unsigned char mcp23x17_address, int handle);

    int mcp23x17_setup(int spi, MCP23x17_ADDRESS mcp23x17_address, int mcp23x17_inta_pin, int mcp23x17_intb_pin); // pass -1 if inta/intb are not used

    MCP23x17_GPIO    mcp23x17_getGPIO(MCP23x17_ADDRESS address, MCP23x17_PORT port, MCP23x17_PIN pin);
    MCP23x17_PIN     mcp23x17_getPin(MCP23x17_GPIO gpio);
    MCP23x17_PORT    mcp23x17_getPort(MCP23x17_GPIO gpio);
    MCP23x17_ADDRESS mcp23x17_getAddress(MCP23x17_GPIO gpio);

    void mcp23x17_setPinInputMode(MCP23x17_GPIO gpio, int enablePullUp, void (*function)(MCP23x17_GPIO pin, int value));

    void mcp23x17_setPinOutputMode(MCP23x17_GPIO gpio, int initialValue);

    unsigned char mcp23x17_virtualReadPort(MCP23x17_ADDRESS address, MCP23x17_PORT port);
    
    int mcp23x17_virtualRead(MCP23x17_GPIO gpio);

    void mcp23x17_digitalWrite(MCP23x17_GPIO gpio, int value);

    int  mcp23x17_digitalRead(MCP23x17_GPIO gpio);

    void mcp23x17_setVirtualPinValue(MCP23x17_GPIO gpio, int value);

    int mcp23x17_bankAddress(int port, int address);

    void mcp23x17_openHandle(MCP23x17_ADDRESS address);

    MCP23x17_GPIO getEnvMCP23x17_GPIO(const char* var);

#ifdef __cplusplus
}  // extern "C"
#endif
