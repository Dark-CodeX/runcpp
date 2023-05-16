#include "../command_line/command_line.hh"

int main(int argc, const char **argv)
{
    if (!runcpp::command_line::parse_command_line(argc, argv))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}