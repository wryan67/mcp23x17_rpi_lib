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


DYN_VERS_MAJ=1
DYN_VERS_MIN=0

VERSION=$(DYN_VERS_MAJ).$(DYN_VERS_MIN)
DESTDIR=/usr
PREFIX=/local

STATIC=libwiringPiMCP23x17.a
DYNAMIC=libwiringPiMCP23x17.so.$(VERSION)

# Use this line for debugging:
# DEBUG	= -g -O0 -rdynamic
# Production
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
CFLAGS	= $(DEBUG) -Wformat=2 -Wall $(INCLUDE) -Winline -pipe -fPIC

LIBS    =

###############################################################################

SRC	= mcp23x17.c mcp23x17_threads.c

OBJ	=	$(SRC:.c=.o)

all:		$(DYNAMIC)

static:		$(STATIC)

$(STATIC):	$(OBJ)
	@echo "[Link (Static)]"
	@ar rcs $(STATIC) $(OBJ)
	@ranlib $(STATIC)
#	@size   $(STATIC)

$(DYNAMIC):	$(OBJ)
	@echo "[Link (Dynamic)]"
	@$(CC) -shared -Wl,-soname,libwiringPiMCP23x17.so -o libwiringPiMCP23x17.so.$(VERSION) -lpthread $(OBJ)

.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "[Clean]"
	@rm -f $(OBJ) $(OBJ_I2C) *~ core tags Makefile.bak libwiringPiMCP23x17.*

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)


install-headers:
	@echo "[Install Headers]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp23x17.h		$(DESTDIR)$(PREFIX)/include

install:	$(DYNAMIC) install-headers
	@echo "[Install Dynamic Lib]"
	@install -m 0755 -d						$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPiMCP23x17.so.$(VERSION)			$(DESTDIR)$(PREFIX)/lib/libwiringPiMCP23x17.so.$(VERSION)
	@ln -sf $(DESTDIR)$(PREFIX)/lib/libwiringPiMCP23x17.so.$(VERSION)	$(DESTDIR)/lib/libwiringPiMCP23x17.so
	@ldconfig

install-static:	$(STATIC) install-headers
	@echo "[Install Static Lib]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPiMCP23x17.a	$(DESTDIR)$(PREFIX)/lib

uninstall:
	@echo "[UnInstall]"
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp23x17.h
	@rm -f $(DESTDIR)$(PREFIX)/lib/libwiringPiMCP23x17.*
	@ldconfig


depend:
	makedepend -Y $(SRC)


libwiringPiMCP23x17.o: mcp23x17.h
