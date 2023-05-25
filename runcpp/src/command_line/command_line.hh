#ifndef RUNCPP_COMMAND_LINE_HH
#define RUNCPP_COMMAND_LINE_HH

#include <openutils/sstring/sstring.hh>
#include <openutils/vector/vector.hh>
#include <openutils/chunkio/chunkio_writer.hh>
#include <openutils/date-time/date.hh>
#include <openutils/date-time/time.hh>

#include "../caller/caller.hh"
#include "../parser/parser.hh"
#include "../io/io.hh"
#include "../os/os.h"
#include "../help/help.h"

namespace runcpp
{
    class command_line
    {
    public:
        [[nodiscard]] static bool generate_config_helper();

        [[nodiscard]] static bool run_command(const openutils::sstring &file_loc, const openutils::vector_t<openutils::sstring> &args, bool __print__);

        [[nodiscard]] static bool run_command_des(const openutils::sstring &file_loc, const openutils::vector_t<openutils::sstring> &args, bool __print__);

        static bool parse_command_line(int argc, const char **argv);

        static bool parse_command_line(const std::size_t &argc, const openutils::sstring *argv);
    };

    bool command_line::generate_config_helper()
    {
        if (io::file_exists("./compile.rc"))
        {
            std::puts("'./compile.rc' file already exists, would you like to overwrite it? [yn] ");
            openutils::sstring overwrite__;
            overwrite__.in().to_lower();
            if (overwrite__ == "n")
                return true;
            else if (overwrite__ == "y")
            {
                openutils::sstring overw_file = "";
                if (!overw_file.save("./compile.rc"))
                {
                    std::fprintf(stderr, "\033[1;91merr:\033[0m could not overwrite the file './compile.rc'\n");
                    return false;
                }
            }
            else
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m invalid choice '%s'\n", overwrite__.c_str());
                return false;
            }
        }
        openutils::chunkio_writer<char> writer("./compile.rc");
        if (writer.file_created() == false)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m could not open file './compile.rc' for writing.\n");
            return false;
        }
        openutils::sstring content, uinput;
        std::printf("Compiler (g++): ");
        uinput.in();
        if (uinput.length() == 0)
            uinput = "g++";
        content = openutils::sstring("[compile]:\n    compiler = '") + uinput + openutils::sstring("'\n");

        if (!writer.append_or_save(content.c_str(), content.length()))
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m file './compile.rc' could not be saved.\n");
            return false;
        }
        uinput.clear();
        content.clear();

        std::printf("Arguments (-g -Wall): ");
        uinput.in();
        if (uinput.length() == 0)
            uinput = "-g -Wall";

        openutils::vector_t<openutils::sstring> vec = uinput.split(" ");
        content = "    args = [";
        for (std::size_t i = 0; i < vec.length(); i++)
        {
            content.append_formatted(1024, "'%s'%s", vec[i].c_str(), (i < vec.length() - 1 ? ", " : ""));
        }
        content += "]\n";

        if (!writer.append_or_save(content.c_str(), content.length()))
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m file './compile.rc' could not be saved.\n");
            return false;
        }
        uinput.clear();
        content.clear();

        std::printf("Sources (./main.cc): ");
        uinput.in();
        if (uinput.length() == 0)
            uinput = "./main.cc";

        vec = uinput.split(" ");
        content = "    sources = [";
        for (std::size_t i = 0; i < vec.length(); i++)
        {
            content.append_formatted(1024, "'%s'%s", vec[i].c_str(), (i < vec.length() - 1 ? ", " : ""));
        }
        content += "]\n";

        if (!writer.append_or_save(content.c_str(), content.length()))
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m file './compile.rc' could not be saved.\n");
            return false;
        }
        uinput.clear();
        content.clear();

        std::printf("Output (./main.out): ");
        uinput.in();
        if (uinput.length() == 0)
            uinput = "./main.out";
        uinput.append_start("-o ");

        vec = uinput.split(" ");
        content = "    output = [";
        for (std::size_t i = 0; i < vec.length(); i++)
        {
            content.append_formatted(1024, "'%s'%s", vec[i].c_str(), (i < vec.length() - 1 ? ", " : ""));
        }
        content += "]\n\n[all]:\n    compile()";

        if (!writer.append_or_save(content.c_str(), content.length()))
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m file './compile.rc' could not be saved.\n");
            return false;
        }
        uinput.clear();
        content.clear();

        std::puts("\nFile './compile.rc' was saved.\nYou can use 'runcpp' to compile your program.");
        return true;
    }

    bool command_line::run_command(const openutils::sstring &file_loc, const openutils::vector_t<openutils::sstring> &args, bool __print__)
    {
        parser ps(file_loc);
        if (!ps.perform_parsing(10))
            return false;
        if (__print__)
            ps.print();
        for (std::size_t i = 0; i < args.length(); i++)
        {
            if (!ps.contains_key(args[i]))
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m target '%s' was not found.\n", args[i].c_str());
                return false;
            }
            if (args[i] != "all")
            {
                openutils::vector_t<openutils::sstring> cmds = ps.generate_command(args[i]);
                std::printf("\033[1;94m%s %s:\033[0m\n", openutils::date().to_string().c_str(), openutils::time().to_string().c_str());
                for (std::size_t j = 0; j < cmds.length(); j++)
                    std::printf("%s%s", cmds[j].c_str(), (j < cmds.length() - 1 ? " " : "\n"));
                caller call(cmds);
                call.init();
            }
            else
            {
                const openutils::vector_t<openutils::vector_t<openutils::sstring>> &cmds = ps.generate_commands_all();
                for (std::size_t k = 0; k < cmds.length(); k++)
                {
                    std::printf("\033[1;94m%s %s:\033[0m\n", openutils::date().to_string().c_str(), openutils::time().to_string().c_str());
                    for (std::size_t j = 0; j < cmds[k].length(); j++)
                        std::printf("%s%s", cmds[k][j].c_str(), (j < cmds[k].length() - 1 ? " " : "\n"));
                    caller call(cmds[k]);
                    call.init();
                }
            }
        }
        return true;
    }

    bool command_line::run_command_des(const openutils::sstring &file_loc, const openutils::vector_t<openutils::sstring> &args, bool __print__)
    {
        parser ps;
        if (!parser::deserialize(ps, file_loc))
            return false;
        if (__print__)
            ps.print();
        for (std::size_t i = 0; i < args.length(); i++)
        {
            if (!ps.contains_key(args[i]))
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m target '%s' was not found.\n", args[i].c_str());
                return false;
            }
            if (args[i] != "all")
            {
                openutils::vector_t<openutils::sstring> cmds = ps.generate_command(args[i]);
                std::printf("\033[1;94m%s %s:\033[0m\n", openutils::date().to_string().c_str(), openutils::time().to_string().c_str());
                for (std::size_t j = 0; j < cmds.length(); j++)
                    std::printf("%s%s", cmds[j].c_str(), (j < cmds.length() - 1 ? " " : "\n"));
                caller call(cmds);
                call.init();
            }
            else
            {
                const openutils::vector_t<openutils::vector_t<openutils::sstring>> &cmds = ps.generate_commands_all();
                for (std::size_t k = 0; k < cmds.length(); k++)
                {
                    std::printf("\033[1;94m%s %s:\033[0m\n", openutils::date().to_string().c_str(), openutils::time().to_string().c_str());
                    for (std::size_t j = 0; j < cmds[k].length(); j++)
                        std::printf("%s%s", cmds[k][j].c_str(), (j < cmds[k].length() - 1 ? " " : "\n"));
                    caller call(cmds[k]);
                    call.init();
                }
            }
        }
        return true;
    }

    bool command_line::parse_command_line(int argc, const char **argv)
    {
        if (argc == 1)
        {
            if (io::file_exists("./compile.rc"))
            {
                return command_line::run_command("./compile.rc", {"all"}, false);
            }
            else
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m no arguments were passed, try using '--help'\n");
                return false;
            }
        }
        openutils::sstring a1 = argv[1];
        if (a1 == "--help" || a1 == "-h")
        {
            std::puts(runcpp_help);
            return true;
        }
        else if (a1 == "--libs" || a1 == "-l")
        {
            std::puts(runcpp_libs);
            return true;
        }
        else if (a1 == "--version" || a1 == "-v")
        {
            std::printf("%s: %s\n", argv[0], runcpp_version);
            return true;
        }
        else if (a1 == "--generate" || a1 == "-gen")
        {
            return command_line::generate_config_helper();
        }
        else if (a1 == "--file" || a1 == "-f")
        {
            if (argc == 2)
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a2 = argv[2]; // file location or print
            if (a2 == "--print" || a2 == "-p")
            {
                if (argc == 3)
                {
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                    return false;
                }
                openutils::sstring a3 = argv[3]; // file location
                if (argc == 4)
                {
                    // means only file loc was passed not target hence only 'all' target is called by default
                    return command_line::run_command(a3, {"all"}, true);
                }
                else
                {
                    openutils::vector_t<openutils::sstring> targets(argc - 4);
                    for (int i = 4; i < argc; i++)
                        targets.add(argv[i]);
                    return command_line::run_command(a3, targets, true);
                }
            }
            else
            {
                // treating a2 as file location
                if (argc == 3)
                {
                    // run only 'all' target
                    return command_line::run_command(a2, {"all"}, false);
                }
                else
                {
                    openutils::vector_t<openutils::sstring> targets(argc - 3);
                    for (int i = 3; i < argc; i++)
                        targets.add(argv[i]);
                    return command_line::run_command(a2, targets, false);
                }
            }
        }
        else if (a1 == "--serialize" || a1 == "-s")
        {
            if (argc == 2)
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a2 = argv[2]; // config file location
            if (argc == 3)
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a3 = argv[3]; // -o flag
            if (a3 != "-o")
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m expected '-o', instead got '%s'\n", a3.c_str());
                return false;
            }
            if (argc == 4)
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a4 = argv[4]; // outfile file location
            // start real job
            parser ps(a2);
            if (!ps.perform_parsing(10))
                return false;
            return parser::serialize(ps, a4);
        }
        else if (a1 == "--deserialize" || a1 == "-d")
        {
            if (argc == 2)
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a2 = argv[2]; // file location or print
            if (a2 == "--print" || a2 == "-p")
            {
                if (argc == 3)
                {
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                    return false;
                }
                openutils::sstring a3 = argv[3]; // file location
                if (argc == 4)
                {
                    // means only file loc was passed not target hence only 'all' target is called by default
                    return command_line::run_command_des(a3, {"all"}, true);
                }
                else
                {
                    openutils::vector_t<openutils::sstring> targets(argc - 4);
                    for (int i = 4; i < argc; i++)
                        targets.add(argv[i]);

                    return command_line::run_command_des(a3, targets, true);
                }
            }
            else
            {
                // treating a2 as file location
                if (argc == 3)
                {
                    // run only 'all' target
                    return command_line::run_command_des(a2, {"all"}, false);
                }
                else
                {
                    openutils::vector_t<openutils::sstring> targets(argc - 3);
                    for (int i = 3; i < argc; i++)
                        targets.add(argv[i]);
                    return command_line::run_command_des(a2, targets, false);
                }
            }
        }
        else if (a1 == "--print" || a1 == "-p")
        {
            if (argc == 2)
            {
                // means run only 'all' target using only ./compile.rc file
                return command_line::run_command("./compile.rc", {"all"}, true);
            }
            else
            {
                openutils::vector_t<openutils::sstring> targets(argc - 2);
                for (int i = 2; i < argc; i++)
                    targets.add(argv[i]);
                return command_line::run_command("./compile.rc", targets, true);
            }
        }
        else
        {
            // treat every arguments as a target
            openutils::vector_t<openutils::sstring> targets(argc - 1);
            for (int i = 1; i < argc; i++)
                targets.add(argv[i]);
            return command_line::run_command("./compile.rc", targets, false);
        }
    }

    bool command_line::parse_command_line(const std::size_t &argc, const openutils::sstring *argv)
    {
        openutils::vector_t<const char *> c_str_argv(argc);
        for (std::size_t i = 0; i < argc; i++)
        {
            c_str_argv.add(argv[i].c_str());
        }
        return command_line::parse_command_line(static_cast<int>(argc), c_str_argv.raw_data());
    }
}

#endif