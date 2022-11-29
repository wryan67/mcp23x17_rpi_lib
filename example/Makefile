all: example knobtest

example: main.cpp
	gcc -lpthread -lwiringPi -lwiringPiMCP23x17rpi main.cpp -o example

knobtest: knobtest.cpp Options.o
	gcc -lpthread -lwiringPi -lwiringPiMCP23x17rpi knobtest.cpp -o knobtest Options.o

Options.o: Options.cpp Options.h
	gcc -c -lpthread -lwiringPi -lwiringPiMCP23x17rpi Options.cpp


clean:
	rm -f example

