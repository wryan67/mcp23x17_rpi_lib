#pragma once

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#include <mcp23x17rpi.h>



class Options {
public:
  char  action='r';
  bool  debug = false;
  bool  reset = false;
  bool  verbose = false;
  int   i2cAddress = 0x20;
  int   port = -1;
  char  modeName[32];
  char  mode;
  int   pin=-1;
  int   value = -1;
  int   minPin=-1;
  int   maxPin=7;

// methods
  void usage();

  bool commandLineOptions(int argc, char ** argv);

};

