#include "Options.h"

void Options::usage() {
    fprintf(stderr, "usage: mcp23017 -i i2c_address\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "  -a = port-a [pin]\n");
    fprintf(stderr, "  -b = port-b [pin]\n");
    fprintf(stderr, "  -c = configuration\n");
    fprintf(stderr, "  -d = debug\n");
    fprintf(stderr, "  -i = i2c address; default=0x20\n");
    fprintf(stderr, "  -m = mode input/output\n");
    fprintf(stderr, "  -o = read olat\n");
    fprintf(stderr, "  -r = read pin values\n");
    fprintf(stderr, "  -v = verbose\n");
    fprintf(stderr, "  -w = write 0/1\n");
    fprintf(stderr, "  -x = reset device\n");
    fprintf(stderr, "Note:  Using write automatically forces\n");
    fprintf(stderr, "       output mode on the specified pin\n");
    fprintf(stderr, "Caution:  Reading the GPIO pins will\n");
    fprintf(stderr, "          clear the interrupts\n");
    fflush(stderr);
    exit(1);
}

bool Options::commandLineOptions(int argc, char** argv) {
    int c, index;

    if (argc < 2) {
        usage();
    }

    const char* shortOptions = "a:b:Lcdi:m:op:rvw:x";

    static struct option longOptions[] = {
      {"porta",          optional_argument, NULL, 'a'},
      {"portb",          optional_argument, NULL, 'b'},
      {"configuration",  optional_argument, NULL, 'c'},
      {"i2c",            optional_argument, NULL, 'i'},
      {"mode",           optional_argument, NULL, 'm'},
      {"olat",           optional_argument, NULL, 'o'},
      {"read",           optional_argument, NULL, 'r'},
      {"verbose",        optional_argument, NULL, 'v'},
      {"write",          optional_argument, NULL, 'w'},
      {"reset",          optional_argument, NULL, 'x'},

      {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, shortOptions, longOptions, &index)) != -1) {

        switch (c) {
        case 'a':
        {
            port = MCP23x17_PORTA;
            minPin = 0;
            sscanf(optarg, "%d", &pin);
            break;
        }

        case 'b':
        {
            port = MCP23x17_PORTB;
            minPin = 0;
            sscanf(optarg, "%d", &pin);
            break;
        }

        case 'c':
        {
            action = 'c';
            break;
        }
        case 'd':
        {
            printf("debugging mode enabled\n");
            debug = true;
            mcp23x17_setDebug(true);
            break;
        }
        case 'i':
        {
            sscanf(optarg, "%x", &i2cAddress);
            break;
        }

        case 'm':
        {
            action = 'm';
            sscanf(optarg, "%s", &modeName);
            mode = tolower(modeName[0]);
            break;
        }

        case 'o':
            action = 'o';
            break;

        case 'p':
            minPin = 0;
            sscanf(optarg, "%d", &pin);
            break;

        case 'r':
            action = 'r';
            break;

        case 'v':
            verbose = true;
            break;

        case 'w':
            action = 'w';
            sscanf(optarg, "%d", &value);
            break;

        case 'x':
            reset = true;
            break;


        case '?':
            if (optopt == 'm' || optopt == 't')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character \\x%x.\n", optopt);

            usage();

        default:
            usage();
        }
    }
    //  for (int index = optind; index < argc; index++)
    //    printf("Non-option argument %s\n", argv[index]);


    if (minPin > pin || pin > maxPin) {
        fprintf(stderr, "invalid pin number\n");
        usage();
    }

    if (-1 > value || value > 1) {
        fprintf(stderr, "invalid value specified\n");
        usage();
    }


    fflush(stdout);
    return true;
}

