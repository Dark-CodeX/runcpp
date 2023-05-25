/**
 * @file help.h
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#ifndef RUNCPP_HELP_H
#define RUNCPP_HELP_H

static const char *runcpp_help = "Usage: [options] [targets]\n"
                                 "Options:\n"
                                 "    --help, -h                Display this help message.\n"
                                 "    --libs, -l                Show the list of used libraries.\n"
                                 "    --version, -v             Display the version of the app.\n"
                                 "    --generate, -gen          Generate a configuration file by taking user input.\n"
                                 "    --print, -p               Print the parsed output.\n"
                                 "    --file, -f                Specify the file to use.\n"
                                 "    --serialize, -s           Serialize the configuration file in binary format.\n"
                                 "    --deserialize, -d         Deserialize the configuration file.\n"
                                 "    --init, -i                Start the interactive mode.\n"
                                 "\nSyntax Rules:\n"
                                 "--print, -p:\n"
                                 "    runcpp --print [targets]\n"
                                 "--file, -f:\n"
                                 "    runcpp --file --print <path_to_file> [targets]\n"
                                 "--serialize, -s:\n"
                                 "    runcpp --serialize <path_to_input_file> -o <path_to_output_file>\n"
                                 "--deserialize, -d:\n"
                                 "    runcpp --deserialize --print <path_to_input_file> [targets]\n"
                                 "\nNotes:\n"
                                 "1. If no arguments are provided by the user, 'runcpp' will try to use 'all' target only.\n"
                                 "2. In flags (i.e. '--file' and '--deserialize'), you can use '--print' flag which is optional.\n"
                                 "3. You can use short-hand versions of above flags.\n"
                                 "4. For any contribution to 'runcpp', visit: https://github.com/Dark-CodeX/runcpp.git\n"
                                 "5. For any bug report, visit: https://github.com/Dark-CodeX/runcpp/issues\n";

static const char *runcpp_libs = "Copyright 2023 Tushar Chaurasia\n"
                                 "    1. [sstring v" OPENUTILS_sstring_version "]      (https://github.com/Dark-CodeX/sstring.git)        Tushar Chaurasia\n"
                                 "    2. [vector v" OPENUTILS_vector_t_version "]       (https://github.com/Dark-CodeX/vector.git)         Tushar Chaurasia\n"
                                 "    3. [date-time v" OPENUTILS_date_time_version "]    (https://github.com/Dark-CodeX/date-time.git)      Tushar Chaurasia\n"
                                 "    4. [chunkio v" OPENUTILS_chunkio_version "]      (https://github.com/Dark-CodeX/chunkio.git)        Tushar Chaurasia\n"
                                 "    5. [heap-pair v" OPENUTILS_heap_pair_version "]    (https://github.com/Dark-CodeX/heap-pair.git)      Tushar Chaurasia";

static const char *runcpp_version = "v1.0";

#endif