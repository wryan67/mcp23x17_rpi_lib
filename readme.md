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

This Library is based on [WiringPi](https://github.com/WiringPi), so, you'll need make sure you 
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


## <a name="utility"></a>Utility

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


## <a name="example"></a>Example

The example program "main.cpp" shows how to use the library to both detect events on input pins as well as modify the output on other pins.  belown is a sample of the program running while the switches are being tripped on/off.

    $ ./example 
    enable txs0108
    initialization complete
    init input pins
    init led to match switch, value = 0
    event pin<a0> value=1
    event pin<a0> value=0
    event pin<a0> value=1
    event pin<a0> value=0


![example1](https://raw.githubusercontent.com/wryan67/mcp23x17_rpi_lib/master/readme/0147360e29a8bb1fad6939c6808a00bebf11492ea2.jpg)
![example2](https://raw.githubusercontent.com/wryan67/mcp23x17_rpi_lib/master/readme/01b87857856baf57ba885609b2ff861285b8ca0b5b.jpg)

![example circuit](https://raw.githubusercontent.com/wryan67/mcp23x17_rpi_lib/master/readme/mcp23017_example.png)

## video example

https://youtu.be/Dp4egt5o8NM
