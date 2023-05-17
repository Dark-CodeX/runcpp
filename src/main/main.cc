#include "../command_line/command_line.hh"

int main(int argc, const char **argv)
{
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
                if (args_broken[0] == "--exit" || args_broken[0] == "-e" || args_broken[0] == "--quit" || args_broken[0] == "-q")
                    return EXIT_SUCCESS;
                runcpp::command_line::parse_command_line(args_broken.length(), args_broken.raw_data());
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