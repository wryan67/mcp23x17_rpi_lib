all: example knobtest lcd1602 latch intlatch

example: main.cpp
	gcc main.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -o example

lcd1602: lcd1602.cpp Options.o
	gcc lcd1602.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -lwiringPiLCD101rpi -lwiringPiDev -o lcd1602 Options.o

latch: latch.cpp LatchOptions.o
	gcc latch.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -lwiringPiLCD101rpi -lwiringPiDev -o latch LatchOptions.o

intlatch: latch.cpp LatchOptions.o
	gcc isrLatch.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -lwiringPiLCD101rpi -lwiringPiDev -o isrlatch LatchOptions.o

knobtest: knobtest.cpp lcd1602.cpp Options.o
	gcc knobtest.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -o knobtest Options.o

Options.o: Options.cpp Options.h
	gcc -c Options.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi 

LatchOptions: LatchOptions.cpp LatchOptions.h
	gcc -c LatchOptions.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi 

clean:
	rm -f example knobtest latch lcd1602 isrlatch *.o

