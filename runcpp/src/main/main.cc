/**
 * @file main.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "../command_line/command_line.hh"
#include <signal.h>

void exit_handler(int signal)
{
    std::fprintf(stderr, "\n\033[1;91merr:\033[0m aborting all tasks and exiting with error code %d.\n", signal);
    std::exit(signal);
}

void segfault_handler(int signal) /* treating segfault different because it should never happen in any case */
{
    std::fprintf(stderr, "\n\033[1;91merr:\033[0m invalid or un-allocated memory access with error code %d, report this error on https://github.com/Dark-CodeX/runcpp/issues\n", signal);
    std::exit(signal);
}

int main(int argc, const char **argv)
{
    signal(SIGINT, exit_handler);
    signal(SIGABRT, exit_handler);
    signal(SIGTERM, exit_handler);
#if defined __linux__ || defined linux || defined __linux
    signal(SIGKILL, exit_handler);
    signal(SIGSTOP, exit_handler);
#endif
    signal(SIGSEGV, segfault_handler);

    if (argc == 1)
    {
        if (!runcpp::command_line::parse_command_line(argc, argv))
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }
    else
    {
        openutils::sstring a1 = argv[1];
        if (a1 == "--init" || a1 == "-i")
        {
            while (true)
            {
                std::printf("/> ");
                openutils::sstring cmds;
                cmds.in();
                openutils::vector_t<openutils::sstring> args_broken = cmds.to_argv(argv[0]);
                if (args_broken.length() == 1)
                {
                    runcpp::command_line::parse_command_line(args_broken.length(), args_broken.raw_data());
                }
                else
                {
                    if (args_broken[1] == "--exit" || args_broken[1] == "-e" || args_broken[1] == "--quit" || args_broken[1] == "-q")
                        return EXIT_SUCCESS;
                    runcpp::command_line::parse_command_line(args_broken.length(), args_broken.raw_data());
                }
            }
        }
        else
        {
            if (!runcpp::command_line::parse_command_line(argc, argv))
                return EXIT_FAILURE;
            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}