all: example knobtest lcd1602 latch

example: main.cpp
	gcc main.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -o example

lcd1602: lcd1602.cpp Options.o
	gcc lcd1602.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -lwiringPiLCD101rpi -lwiringPiDev -o lcd1602 Options.o

latch: latch.cpp Options.o
	gcc latch.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -lwiringPiLCD101rpi -lwiringPiDev -o latch Options.o


knobtest: knobtest.cpp lcd1602.cpp Options.o
	gcc knobtest.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -o knobtest Options.o

Options.o: Options.cpp Options.h
	gcc -c Options.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi 


clean:
	rm -f example

