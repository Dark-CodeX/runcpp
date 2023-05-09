#include "../parser/parser.hh"
#include "../io/io.hh"
#include "../caller/caller.hh"
#include <openutils/date-time/date.hh>
#include <openutils/date-time/time.hh>
#include <openutils/chunkio/chunkio_writer.hh>
#include "./help/help.h"

static inline int init_parser_and_caller(const openutils::sstring &file_loc, const openutils::vector_t<openutils::sstring> &keys = {"global_s"})
{
    if (runcpp::io::file_exists(file_loc))
    {
        openutils::sstring content = runcpp::io::read_file(file_loc);

        runcpp::parser parser(content, file_loc);
        parser.perform_parsing();
        for (std::size_t i = 0; i < keys.length(); i++)
        {
            if (!parser.contains_key(keys[i]))
            {
                std::fprintf(stderr, "err: target `%s` not found.\n", keys[i].c_str());
                return EXIT_FAILURE;
            }
        }

        openutils::vector_t<openutils::vector_t<openutils::sstring>> commands;
        for (std::size_t i = 0; i < keys.length(); i++)
        {
            commands.add(parser.generate_command(keys[i]));
        }
        parser.clear_memory();

        for (std::size_t i = 0; i < commands.length(); i++)
        {
            std::cout << openutils::date().to_string() << " " << openutils::time().to_string() << ":\n";

            for (std::size_t j = 0; j < commands[i].length(); j++)
                std::cout << commands[i][j] << (j < commands[i].length() - 1 ? " " : "\n");
            runcpp::caller caller(commands[i]);

            caller.init();
        }
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        if (init_parser_and_caller("./compile.rc") == EXIT_FAILURE)
        {
            std::fprintf(stderr, "err: no argument, use `--help` flag to see help.\n");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    openutils::sstring argv_1 = argv[1];
    if (argv_1 == "--help")
    {
        std::puts(help);
        return EXIT_SUCCESS;
    }
    else if (argv_1 == "--libs")
    {
        std::puts(libs);
        return EXIT_SUCCESS;
    }
    else if (argv_1 == "--version")
    {
        std::printf("%s: %s\n", argv[0], version);
        return EXIT_SUCCESS;
    }
    else if (argv_1 == "--gen" || argv_1 == "--generate")
    {
        if (runcpp::io::file_exists("./compile.rc"))
        {
            std::cout << "`./compile.rc` file already exists, would you like to overwrite it? [yn] ";
            openutils::sstring overwrite__;
            overwrite__.in().to_lower();
            if (overwrite__ == "n")
                return EXIT_SUCCESS;
            else if (overwrite__ == "y")
            {
                openutils::sstring overw_file = "";
                overw_file.save("./compile.rc");
            }
            else
            {
                std::fprintf(stderr, "err: invalid choice `%s`\n", overwrite__.c_str());
                return EXIT_FAILURE;
            }
        }
        openutils::chunkio_writer<char> writer("./compile.rc");
        openutils::sstring content, uinput;
        std::cout << "Compiler (g++): ";
        uinput.in();
        if (uinput.length() == 0)
            uinput = "g++";
        content = openutils::sstring("[global_s]:\ncompiler = \"") + uinput + openutils::sstring("\"\n");

        if (!writer.append_or_save(content.c_str(), content.length()))
        {
            std::fprintf(stderr, "err: file `%s` could not be saved.\n", "./compile.rc");
            return EXIT_FAILURE;
        }
        uinput.clear();
        content.clear();

        std::cout << "Arguments (-g -Wall): ";
        uinput.in();
        if (uinput.length() == 0)
            uinput = "-g -Wall";

        openutils::vector_t<openutils::sstring> vec = uinput.split(" ");
        content = "args = [";
        for (std::size_t i = 0; i < vec.length(); i++)
        {
            content.append_formatted(1024, "\"%s\"%s", vec[i].c_str(), (i < vec.length() - 1 ? ", " : ""));
        }
        content += "]\n";

        if (!writer.append_or_save(content.c_str(), content.length()))
        {
            std::fprintf(stderr, "err: file `%s` could not be saved.\n", "./compile.rc");
            return EXIT_FAILURE;
        }
        uinput.clear();
        content.clear();

        std::cout << "Sources (./main.cc): ";
        uinput.in();
        if (uinput.length() == 0)
            uinput = "./main.cc";

        vec = uinput.split(" ");
        content = "sources = [";
        for (std::size_t i = 0; i < vec.length(); i++)
        {
            content.append_formatted(1024, "\"%s\"%s", vec[i].c_str(), (i < vec.length() - 1 ? ", " : ""));
        }
        content += "]\n";

        if (!writer.append_or_save(content.c_str(), content.length()))
        {
            std::fprintf(stderr, "err: file `%s` could not be saved.\n", "./compile.rc");
            return EXIT_FAILURE;
        }
        uinput.clear();
        content.clear();

        std::cout << "Output (./main.out): ";
        uinput.in();
        if (uinput.length() == 0)
            uinput = "./main.out";
        uinput.append_start("-o ");

        vec = uinput.split(" ");
        content = "output = [";
        for (std::size_t i = 0; i < vec.length(); i++)
        {
            content.append_formatted(1024, "\"%s\"%s", vec[i].c_str(), (i < vec.length() - 1 ? ", " : ""));
        }
        content += "]\n";

        if (!writer.append_or_save(content.c_str(), content.length()))
        {
            std::fprintf(stderr, "err: file `%s` could not be saved.\n", "./compile.rc");
            return EXIT_FAILURE;
        }
        uinput.clear();
        content.clear();

        std::cout << "\nFile "
                  << "./compile.rc"
                  << " saved.\nYou can use `runcpp` to compile your program." << std::endl;
        return EXIT_SUCCESS;
    }
    else if (argv_1 == "--file")
    {
        if (argc == 2)
        {
            std::fprintf(stderr, "err: not enough arguments, use `--help` flag to see help.\n");
            return EXIT_FAILURE;
        }
        openutils::sstring argv_2 = argv[2]; // file loc
        openutils::vector_t<openutils::sstring> vec(argc);
        if (argc == 3)
        {
            vec.add("global_s");
        }
        else
        {
            for (std::size_t i = 3; i < static_cast<std::size_t>(argc); i++)
                vec.add(argv[i]);
        }
        return init_parser_and_caller(argv_2, vec);
    }
    else
    {
        openutils::vector_t<openutils::sstring> vec(argc);
        for (std::size_t i = 1; i < static_cast<std::size_t>(argc); i++)
            vec.add(argv[i]);
        return init_parser_and_caller("./compile.rc", vec);
    }
}