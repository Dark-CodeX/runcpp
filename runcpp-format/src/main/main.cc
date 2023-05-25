/**
 * @file main.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "../formatter/formatter.hh"
#include "../help/help.h"

int main(int argc, const char **argv)
{
    if (argc == 1)
    {
        std::fprintf(stderr, "\033[1;91merr:\033[0m no arguments were passed, try using '--help'\n");
        return EXIT_FAILURE;
    }
    openutils::sstring a1 = argv[1];
    if (a1 == "--help" || a1 == "-h")
    {
        std::puts(runcpp_format_help);
        return EXIT_SUCCESS;
    }
    else if (a1 == "--libs" || a1 == "-l")
    {
        std::puts(runcpp_format_libs);
        return EXIT_SUCCESS;
    }
    else if (a1 == "--version" || a1 == "-v")
    {
        std::printf("%s: %s\n", argv[0], runcpp_format_version);
        return EXIT_SUCCESS;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            runcpp_format::formatter rff(argv[i]);
            if (!rff.do_formatting(10)) // read 10 lines at max
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m could not format the given file '%s'\n", argv[i]);
                return EXIT_FAILURE;
            }
            if (!rff.save_or_overwrite())
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m could not overwrite the formatted file at '%s'\n", argv[i]);
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}