#ifndef HELP_HH
#define HELP_HH

static const char *runcpp_help = "Usage: runcpp [target]\n"
                          "Options:\n"
                          "    --help        Shows this message and exit.\n"
                          "    --libs        Shows libraries used and exit.\n"
                          "    --version     Shows current version and exit.\n"
                          "    --file        Reads FILE as a 'compile.rc' file.\n"
                          "    --gen         Generates a 'compile.rc' file for C/C++ by taking user input.\n"
                          "    --generate    Does same job as '--gen' flag.";

static const char *runcpp_libs = "Copyright 2023 Tushar Chaurasia\n"
                          "    1. [sstring v" OPENUTILS_sstring_version "]      (https://github.com/Dark-CodeX/sstring.git)        Tushar Chaurasia\n"
                          "    2. [vector v" OPENUTILS_vector_t_version "]       (https://github.com/Dark-CodeX/vector.git)         Tushar Chaurasia\n"
                          "    3. [date-time v" OPENUTILS_date_time_version "]    (https://github.com/Dark-CodeX/date-time.git)      Tushar Chaurasia\n"
                          "    4. [chunkio v" OPENUTILS_chunkio_version "]      (https://github.com/Dark-CodeX/chunkio.git)        Tushar Chaurasia\n"
                          "    5. [heap-pair v" OPENUTILS_heap_pair_version "]    (https://github.com/Dark-CodeX/heap-pair.git)      Tushar Chaurasia";

static const char *runcpp_version = "v1.0";

#endif