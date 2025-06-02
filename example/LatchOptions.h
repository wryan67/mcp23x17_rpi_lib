#pragma once

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#include <mcp23x17rpi.h>



class LatchOptions {
public:
  char  action='r';
  bool  debug = false;
  bool  verbose = false;
  int   i2cAddress = 0x20;
  int   inta=-1;
  int   intb=-1;
  bool  latch=true;

// methods
  void usage();

  bool commandLineOptions(int argc, char ** argv);

};

