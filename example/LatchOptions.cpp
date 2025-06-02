#include "LatchOptions.h"

void LatchOptions::usage() {
    fprintf(stderr, "usage: mcp23017 -i i2c_address\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "  -a = int-a gpio\n");
    fprintf(stderr, "  -b = int-b gpio\n");
    fprintf(stderr, "  -d = debug\n");
    fprintf(stderr, "  -i = i2c address; default=0x20\n");
    fprintf(stderr, "  -l = don't latch\n");
    fprintf(stderr, "  -v = verbose\n");
    fflush(stderr);
    exit(1);
}

bool LatchOptions::commandLineOptions(int argc, char** argv) {
    int c, index;

    const char* shortOptions = "a:b:di:lv";

    static struct option longOptions[] = {
      {"porta",          optional_argument, NULL, 'a'},
      {"portb",          optional_argument, NULL, 'b'},
      {"i2c",            optional_argument, NULL, 'i'},
      {"verbose",        optional_argument, NULL, 'v'},

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

            case 'l': { // dont latch
                latch=false;
                break;
            }




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

