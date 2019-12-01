# Event Driven MCP23017

This softtware library is designed to interface with the MCP2307 and MCP23S17 chips 
that are made by Microchip.  This library makes use of the interrupt pins 
on the mcp23x17 chips to allow the developer to write event driven functions.   

Copyright (c) 2019 Wade Ryan



## Requirements
On your Raspberry Pi, please use the raspi-config program to enable the I2C interface.
Then use the gpio command to make sure your i2c devices can be found.  The default address 
for an MCP23017 chip is 0x20.  

	$ sudo raspi-config
	$ gpio i2cd


## Prerequisites

This Library is based on [WiringPi](http://wiringpi.com/), so, you'll need make sure you 
have WiringPi installed before you can succesfully compile this library.  


## Install

To compile this library, navigate into the src folder and use the make utility to compile 
and install the library.

    $ cd [project folder]
    $ cd src
    $ make && sudo make install
    $ cd ../utility
    $ make && sudo make install

## Compiling
Complie your applications using these command line arguments: -lwiringPi -lwiringPiMCP23x17

## Examples
In the readme folder, there is a suggested wiring diagram based on standard 
CAT5 cabeling.  The normal I2C distance requirements apply, so be sure to 
keep your cable short.

In the example folder, there is a sample C++ program with some additional 
instructions in the heading commments.  

## Usage

There is an accopmaning command line utility, which will allow you to view 
the status of the mcp23017 chip.  Before the Utility can be used, the chip must 
be initialized using the -x option.  Once a chip has been initizalzed, there 
should not be a need to initialize it again, unless you use a different library 
which may change the settings required for this utility to work. 

	$ mcp23x17
	usage: mcp23017 -i i2c_address
	  Options:
	  -a = port-a [pin]
	  -b = port-b [pin]
	  -c = configuration
	  -d = debug
	  -i = i2c address; default=0x20
	  -m = mode input/output
	  -o = read olat
	  -r = read pin values
	  -v = verbose
	  -w = write 0/1
	  -x = reset device
	Note:  Using write automatically forces
		   output mode on the specified pin
	Caution:  Reading the GPIO pins will
			  clear the interrupts



	$ mcp23x17 -xi 20
	$ mcp23x17 -ri 20
	+--------------++--------------+
	|       IIC Address 0x20       |
	+--------------++--------------+
	|    Port-B    ||    Port-A    |
	+---+------+---++---+------+---+
	| n | Mode | V || V | Mode | n |
	+---+------+---++---+------+---+
	| 0 |  OUT | 0 || 1 |  OUT | 7 |
	| 1 |  OUT | 1 || 0 |  OUT | 6 |
	| 2 |  OUT | 0 || 0 |  OUT | 5 |
	| 3 |  OUT | 0 || 0 |  OUT | 4 |
	| 4 |  OUT | 0 || 1 |  IN  | 3 |
	| 5 |  IN  | 1 || 1 |  IN  | 2 |
	| 6 |  IN  | 1 || 1 |  IN  | 1 |
	| 7 |  IN  | 1 || 1 |  IN  | 0 |
	+---+------+---++---+------+---+




## Datasheet

[MCP23017/MCP23S17 Datasheet](http://ww1.microchip.com/downloads/en/devicedoc/20001952c.pdf)
