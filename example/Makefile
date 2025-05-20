all: example knobtest

example: main.cpp
	gcc main.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -o example

knobtest: knobtest.cpp Options.o
	gcc knobtest.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi -o knobtest Options.o

Options.o: Options.cpp Options.h
	gcc -c Options.cpp -lpthread -lwiringPi -lwiringPiMCP23x17rpi 


clean:
	rm -f example

