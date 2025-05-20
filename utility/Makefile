##########################################################################
# Makefile
#
# This software is a devLib extension to wiringPi <http://wiringpi.com/>
# and enables it to control the MCP23x17 16-Bit I/O Expander.
# http://ww1.microchip.com/downloads/en/devicedoc/20001952c.pdf
# 
#
# Copyright (c) 2019 Wade Ryan
#
# If you have questions or improvements email me at
# wryan67@gmail.com
#
# This software is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The given code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You can view the contents of the licence at <http://www.gnu.org/licenses/>.
##########################################################################

DESTDIR=/usr/local/bin

TARGET=mcp23x17


# Use this line for debugging:
# DEBUG	= -g -O0 -rdynamic
# Production
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) 


###############################################################################

SRC = Options.cpp utility.cpp
OBJ	=	$(SRC:.cpp=.o)

all:  $(TARGET)

.cpp.o:
	@echo "[Compile]"
	@$(CC) -c $(CFLAGS) $<  -o $@

$(TARGET): $(OBJ)
	@echo "[Link]"
	$(CC) $(CFLAGS) *.o -lwiringPiMCP23x17rpi -lwiringPi -pthread -o $(TARGET) 

clean:
	@echo "[Clean]"
	@rm -f *.o mcp23x17

    
install:	$(TARGET) 
	@echo "[Install]"
	@cp mcp23x17 $(DESTDIR)


uninstall:
	@echo "[UnInstall]"
	@rm -f $(DESTDIR)/mcp23x17


