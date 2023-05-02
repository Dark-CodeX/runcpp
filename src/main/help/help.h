#ifndef HELP_HH
#define HELP_HH

static const char *help = "Usage: runcpp [target]\n"
                          "Options:\n"
                          "    --help        Shows this message and exit.\n"
                          "    --libs        Shows libraries used and exit.\n"
                          "    --version     Shows current version and exit.\n"
                          "    --file        Reads FILE as a `compile.rc` file.\n"
                          "    --gen         Generates a `compile.rc` file for C/C++ by taking user input.\n"
                          "    --generate    Does same job as `--gen` flag.\n"
                          "    --verbose     Shows the data structure in which input file is parsed and stored.\n";

static const char *libs = "Copyright 2023 Tushar Chaurasia\n"
                              "    1. [sstring v" sstring_version "]      (https://github.com/Dark-CodeX/sstring.git)        Tushar Chaurasia\n"
                              "    2. [vector v" vector_t_version "]       (https://github.com/Dark-CodeX/vector.git)         Tushar Chaurasia\n"
                              "    3. [date-time v" date_time_version "]    (https://github.com/Dark-CodeX/date-time.git)      Tushar Chaurasia\n"
                              "    4. [heap-pair v" heap_pair_version "]    (https://github.com/Dark-CodeX/heap-pair.git)      Tushar Chaurasia";

static const char *version = "v1.0";

#endif