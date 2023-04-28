#include "../parser/parser.hh"
#include "../io/io.hh"
#include "../caller/caller.hh"
#include <openutils/date-time/date.hh>
#include <openutils/date-time/time.hh>
#include "./help/help.h"

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        if (runcpp::io::file_exists("./compile.rc"))
        {
            openutils::sstring cont = runcpp::io::read_file("./compile.rc");
            runcpp::parser parser(cont, "./compile.rc");
            parser.perform_parsing();
            if (!parser.contains_key("global_s"))
            {
                std::fprintf(stderr, "err: target `%s` not found.\n", "global_s");
                return EXIT_FAILURE;
            }
            openutils::vector_t<openutils::sstring> cmds = parser.generate_command("global_s");
            parser.~parser();

            std::cout << openutils::date().to_string() << " " << openutils::time().to_string() << ":\n";
            for (std::size_t i = 0; i < cmds.length(); i++)
                std::cout << cmds[i] << (i < cmds.length() - 1 ? " " : "\n");
            runcpp::caller caller(cmds);

            caller.init();
            return EXIT_SUCCESS;
        }
        std::fprintf(stderr, "err: no argument, use `--help` flag to see help.\n");
        return EXIT_FAILURE;
    }
    openutils::sstring argv_1 = argv[1];
    openutils::sstring cont;
    if (argv_1 == "--help")
    {
        std::puts(help);
        return EXIT_SUCCESS;
    }
    if (argv_1 == "--libs")
    {
        std::puts(libs);
        return EXIT_SUCCESS;
    }
    if (argv_1 == "--version")
    {
        std::printf("%s: %s\n", argv[0], version);
        return EXIT_SUCCESS;
    }
    if (argv_1 == "--gen" || argv_1 == "--generate")
    {
        openutils::sstring content, uinput;
        std::cout << "Compiler (g++): ";
        uinput.in();
        if (uinput.length() == 0)
            uinput = "g++";
        content += openutils::sstring("[global_s]:\ncompiler = \"") + uinput + openutils::sstring("\"\n");

        uinput.clear();

        std::cout << "Arguments (-g -Wall): ";
        uinput.in();
        if (uinput.length() == 0)
            uinput = "-g -Wall";

        openutils::vector_t<openutils::sstring> vec = uinput.split(" ");
        content += "args = [";
        for (std::size_t i = 0; i < vec.length(); i++)
        {
            content.append_formatted(1024, "\"%s\"%s", vec[i].c_str(), (i < vec.length() - 1 ? ", " : ""));
        }
        content += "]\n";

        uinput.clear();

        std::cout << "Sources (./main.cc): ";
        uinput.in();
        if (uinput.length() == 0)
            uinput = "./main.cc";

        vec = uinput.split(" ");
        content += "sources = [";
        for (std::size_t i = 0; i < vec.length(); i++)
        {
            content.append_formatted(1024, "\"%s\"%s", vec[i].c_str(), (i < vec.length() - 1 ? ", " : ""));
        }
        content += "]\n";

        uinput.clear();

        std::cout << "Output (./main.out): ";
        uinput.in();
        if (uinput.length() == 0)
            uinput = "./main.out";
        uinput.append_start("-o ");

        vec = uinput.split(" ");
        content += "output = [";
        for (std::size_t i = 0; i < vec.length(); i++)
        {
            content.append_formatted(1024, "\"%s\"%s", vec[i].c_str(), (i < vec.length() - 1 ? ", " : ""));
        }
        content += "]\n";

        content.shrink_to_fit();

        if (!content.save("./compile.rc"))
        {
            std::fprintf(stderr, "err: file `%s` could not be saved.\n", "./compile.rc");
            return EXIT_FAILURE;
        }
        std::cout << "\nFile "
                  << "./compile.rc"
                  << " saved.\nYou can use `runcpp` to compile your program." << std::endl;
        return EXIT_SUCCESS;
    }
    if (argv_1 == "--file")
    {
        if (argc == 2)
        {
            std::fprintf(stderr, "err: not enough arguments, use `--help` flag to see help.\n");
            return EXIT_FAILURE;
        }
        openutils::sstring argv_2 = argv[2];
        if (!runcpp::io::file_exists(argv_2))
        {
            std::fprintf(stderr, "err: file `%s` not found, try using `--gen` to generate a new `compile.rc` file.\n", argv[2]);
            return EXIT_FAILURE;
        }
        cont = runcpp::io::read_file(argv_2);
    }

    if (cont.is_null())
    {
        if (!runcpp::io::file_exists("./compile.rc"))
        {
            std::fprintf(stderr, "err: file `%s` not found, try using `--gen` to generate a new `compile.rc` file.\n", "./compile.rc");
            return EXIT_FAILURE;
        }
        cont = runcpp::io::read_file("./compile.rc");
    }

    runcpp::parser parser(cont, "./compile.rc");
    parser.perform_parsing();

    if (!parser.contains_key(argv_1))
    {
        std::fprintf(stderr, "err: target `%s` not found.\n", argv_1.c_str());
        return EXIT_FAILURE;
    }

    openutils::vector_t<openutils::sstring> cmds = parser.generate_command(argv_1);
    parser.~parser();

    std::cout << openutils::date().to_string() << " " << openutils::time().to_string() << ":\n";
    for (std::size_t i = 0; i < cmds.length(); i++)
        std::cout << cmds[i] << (i < cmds.length() - 1 ? " " : "\n");
    runcpp::caller caller(cmds);

    caller.init();
    return EXIT_SUCCESS;
}