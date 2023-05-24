#ifndef RUNCPP_FORMAT_HELP_H
#define RUNCPP_FORMAT_HELP_H

static const char *runcpp_format_help = "Usage: [files or options]\n"
                                 "Options:\n"
                                 "    --help, -h                Display this help message.\n"
                                 "    --libs, -l                Show the list of used libraries.\n"
                                 "    --version, -v             Display the version of the app.\n";

static const char *runcpp_format_libs = "Copyright 2023 Tushar Chaurasia\n"
                                        "    1. [sstring v" OPENUTILS_sstring_version "]      (https://github.com/Dark-CodeX/sstring.git)        Tushar Chaurasia\n"
                                        "    2. [vector v" OPENUTILS_vector_t_version "]       (https://github.com/Dark-CodeX/vector.git)         Tushar Chaurasia\n"
                                        "    3. [chunkio v" OPENUTILS_chunkio_version "]      (https://github.com/Dark-CodeX/chunkio.git)        Tushar Chaurasia\n"
                                        "    4. [heap-pair v" OPENUTILS_heap_pair_version "]    (https://github.com/Dark-CodeX/heap-pair.git)      Tushar Chaurasia";

static const char *runcpp_format_version = "v1.0";

#endif