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
                                 "    --libs, -l                Display the list of used libraries.\n"
                                 "    --info, -I                Display the time of compilation and name of compiler.\n"
                                 "    --version, -v             Display the version of the app.\n"
                                 "    --generate, -gen          Generate a configuration file by taking user input.\n"
                                 "    --print, -p               Print the parsed output.\n"
                                 "    --file, -f                Specify the file to use.\n"
                                 "    --print-gui-client        Print the parsed output with target's name and hash, must be used with '--file' or '--serialize' flag.\n"
                                 "    --merge, -m               Resolves any 'import' statements and merges all imported files into a single file.\n"
                                 "    --serialize, -s           Serialize the configuration file in binary format.\n"
                                 "    --deserialize, -d         Deserialize the configuration file.\n"
                                 "    --init, -i                Start the interactive mode.\n"
                                 "\nDocumentation:\n"
                                 "For documentation you can visit: https://dark-codex.github.io/runcpp-docs/\n"
                                 "\nSyntax Rules:\n"
                                 "--print, -p:\n"
                                 "    runcpp --print [targets]\n"
                                 "--file, -f:\n"
                                 "    runcpp --file --print <path_to_file> [targets]\n"
                                 "    runcpp --file --print-gui-client <path_to_file>\n"
                                 "--merge, -m:\n"
                                 "    runcpp --merge <path_to_file> -o <path_to_output_file>\n"
                                 "--serialize, -s:\n"
                                 "    runcpp --serialize <path_to_input_file> -o <path_to_output_file>\n"
                                 "    runcpp --serialize <path_to_input_file> -o <path_to_output_file> --print-gui-client\n"
                                 "--deserialize, -d:\n"
                                 "    runcpp --deserialize --print <path_to_input_file> [targets]\n"
                                 "\nNotes:\n"
                                 "1. If no targets are provided by the user, 'runcpp' will try to use 'all' target only.\n"
                                 "2. In flags (i.e. '--file' and '--deserialize'), you can use '--print' flag which is optional.\n"
                                 "3. You can use short-handed versions of above flags.\n"
                                 "4. For any contribution to 'runcpp', visit: https://github.com/Dark-CodeX/runcpp.git\n"
                                 "5. For any bug report, visit: https://github.com/Dark-CodeX/runcpp/issues\n"
                                 "\nLicense:\n"
                                 "This project is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.";

static const char *runcpp_libs = "Copyright 2023 Tushar Chaurasia\n"
                                 "    1. [sstring v" OPENUTILS_sstring_version "]      (https://github.com/Dark-CodeX/openutils.git)         Tushar Chaurasia\n"
                                 "    2. [vector v" OPENUTILS_vector_t_version "]       (https://github.com/Dark-CodeX/openutils.git)         Tushar Chaurasia\n"
                                 "\nLicense:\n"
                                 "This project is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.";

static const char *runcpp_version = "v1.0.0";

#define helper_pre_processor_num_to_str(num) \
    #num

#define PREPROCESSOR_NUM_TO_STR(num) \
    helper_pre_processor_num_to_str(num)

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
static const char *runcpp_info = "runcpp: compiled on MSVC v" PREPROCESSOR_NUM_TO_STR(_MSC_FULL_VER) " at " __DATE__ " " __TIME__;
#else
#if defined __clang__
static const char *runcpp_info = "runcpp: compiled on Clang v" PREPROCESSOR_NUM_TO_STR(__clang_major__) "." PREPROCESSOR_NUM_TO_STR(__clang_minor__) "." PREPROCESSOR_NUM_TO_STR(__clang_patchlevel__) " at " __DATE__ " " __TIME__;
#else
static const char *runcpp_info = "runcpp: compiled on GCC v" PREPROCESSOR_NUM_TO_STR(__GNUC__) "." PREPROCESSOR_NUM_TO_STR(__GNUC_MINOR__) "." PREPROCESSOR_NUM_TO_STR(__GNUC_PATCHLEVEL__) " at " __DATE__ " " __TIME__;
#endif
#endif

#endif