#include "LatchOptions.h"

void LatchOptions::usage() {
    fprintf(stderr, "usage: mcp23017 -i i2c_address\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "  -a = int-a [pin]\n");
    fprintf(stderr, "  -b = int-b [pin]\n");
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

bool LatchOptions::commandLineOptions(int argc, char** argv) {
    int c, index;



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
            case 'a': {
                sscanf(optarg, "%d", &inta);
                break;
            }
            case 'b': {
                sscanf(optarg, "%d", &intb);
                break;
            }


            case 'c': {
                action = 'c';
                break;
            }

            case 'd': {
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

            case 'o':
                action = 'o';
                break;


            case 'r':
                action = 'r';
                break;

            case 'v':
                verbose = true;
                break;


        case '?':
            if (optopt == 'm' || optopt == 't')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character \\x%x.\n", optopt);

                fprintf(stderr,"tag20\n");
            usage();

        default:

            fprintf(stderr,"unknown option: %c\n",c);
            usage();
        }
    }
    //  for (int index = optind; index < argc; index++)
    //    printf("Non-option argument %s\n", argv[index]);






    fflush(stdout);
    return true;
}

