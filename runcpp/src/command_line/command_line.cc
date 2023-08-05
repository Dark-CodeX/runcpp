/**
 * @file command_line.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./command_line.hh"

namespace runcpp
{
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
                    if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                        std::fprintf(stderr, "\033[1;91merr:\033[0m could not overwrite the file './compile.rc': %s\n", std::strerror(errno));
                    else
                        std::fprintf(stderr, "err: could not overwrite the file './compile.rc': %s\n", std::strerror(errno));
                    return false;
                }
            }
            else
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m invalid choice '%s'\n", overwrite__.c_str());
                else
                    std::fprintf(stderr, "err: invalid choice '%s'\n", overwrite__.c_str());
                return false;
            }
        }
        std::FILE *fptr = std::fopen("./compile.rc", "wb");
        if (!fptr)
        {
            if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m could not open file './compile.rc' for writing: %s\n", std::strerror(errno));
            else
                std::fprintf(stderr, "err: could not open file './compile.rc' for writing: %s\n", std::strerror(errno));
            return false;
        }
        openutils::sstring content, uinput;
        std::printf("Compiler (g++): ");
        uinput.in();
        if (uinput.length() == 0)
            uinput = "g++";
        content = openutils::sstring("[compile]:\n    compiler = '") + uinput + openutils::sstring("'\n");

        if (std::fwrite(content.c_str(), sizeof(char), content.length(), fptr) != content.length())
        {
            if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file './compile.rc' could not be saved: %s\n", std::strerror(errno));
            else
                std::fprintf(stderr, "err: file './compile.rc' could not be saved: %s\n", std::strerror(errno));
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

        if (std::fwrite(content.c_str(), sizeof(char), content.length(), fptr) != content.length())
        {
            if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file './compile.rc' could not be saved: %s\n", std::strerror(errno));
            else
                std::fprintf(stderr, "err: file './compile.rc' could not be saved: %s\n", std::strerror(errno));
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

        if (std::fwrite(content.c_str(), sizeof(char), content.length(), fptr) != content.length())
        {
            if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file './compile.rc' could not be saved: %s\n", std::strerror(errno));
            else
                std::fprintf(stderr, "err: file './compile.rc' could not be saved: %s\n", std::strerror(errno));
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

        if (std::fwrite(content.c_str(), sizeof(char), content.length(), fptr) != content.length())
        {
            if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file './compile.rc' could not be saved: %s\n", std::strerror(errno));
            else
                std::fprintf(stderr, "err: file './compile.rc' could not be saved: %s\n", std::strerror(errno));
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
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m target '%s' was not found.\n", args[i].c_str());
                else
                    std::fprintf(stderr, "err: target '%s' was not found.\n", args[i].c_str());
                return false;
            }
            if (args[i] != "all")
            {
                openutils::vector_t<openutils::sstring> cmds = ps.generate_command(args[i]);
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_OUT))
                    std::printf("\033[1;94m%s:\033[0m\n", command_line::get_date_time().c_str());
                else
                    std::printf("%s:\n", command_line::get_date_time().c_str());
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
                    if (colorize::should_colorize(colorize::STDPTR::STANDARD_OUT))
                        std::printf("\033[1;94m%s:\033[0m\n", command_line::get_date_time().c_str());
                    else
                        std::printf("%s:\n", command_line::get_date_time().c_str());
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
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m target '%s' was not found.\n", args[i].c_str());
                else
                    std::fprintf(stderr, "err: target '%s' was not found.\n", args[i].c_str());
                return false;
            }
            if (args[i] != "all")
            {
                openutils::vector_t<openutils::sstring> cmds = ps.generate_command(args[i]);
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_OUT))
                    std::printf("\033[1;94m%s:\033[0m\n", command_line::get_date_time().c_str());
                else
                    std::printf("%s:\n", command_line::get_date_time().c_str());
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
                    if (colorize::should_colorize(colorize::STDPTR::STANDARD_OUT))
                        std::printf("\033[1;94m%s:\033[0m\n", command_line::get_date_time().c_str());
                    else
                        std::printf("%s:\n", command_line::get_date_time().c_str());
                    for (std::size_t j = 0; j < cmds[k].length(); j++)
                        std::printf("%s%s", cmds[k][j].c_str(), (j < cmds[k].length() - 1 ? " " : "\n"));
                    caller call(cmds[k]);
                    call.init();
                }
            }
        }
        return true;
    }

    openutils::sstring command_line::get_date_time()
    {
        std::time_t t = std::time(nullptr);
        struct std::tm *ct = std::localtime(&t);

        openutils::sstring str;
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
        str.set_formatted(128, "%d/%d/%d %d:%d:%d %s",
                          ct->tm_year + 1900,
                          ct->tm_mon + 1,
                          ct->tm_mday,
                          ((ct->tm_hour + 11) % 12 + 1),
                          ct->tm_min,
                          ct->tm_sec,
                          ((ct->tm_hour >= 12) ? "PM" : "AM"));
#else
        str.set_formatted(128, "%d/%d/%d %d:%d:%d %s %s",
                          ct->tm_year + 1900,
                          ct->tm_mon + 1,
                          ct->tm_mday,
                          ((ct->tm_hour + 11) % 12 + 1),
                          ct->tm_min,
                          ct->tm_sec,
                          ((ct->tm_hour >= 12) ? "PM" : "AM"),
                          ct->tm_zone);
#endif
        return str;
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
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m no arguments were passed, try using '--help'\n");
                else
                    std::fprintf(stderr, "err: no arguments were passed, try using '--help'\n");
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
        else if (a1 == "--info" || a1 == "-I")
        {
            std::puts(runcpp_info);
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
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                else
                    std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a2 = argv[2]; // file location or print
            if (a2 == "--print" || a2 == "-p")
            {
                if (argc == 3)
                {
                    if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                        std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                    else
                        std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
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
            else if (a2 == "--print-gui-client")
            {
                if (argc == 3)
                {
                    if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                        std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                    else
                        std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
                    return false;
                }
                openutils::sstring a3 = argv[3]; // file location
                parser ps(a3, true);
                if (!ps.perform_parsing(10))
                    return false;
                ps.print_for_gui_client();
                return true;
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
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                else
                    std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a2 = argv[2]; // config file location
            if (argc == 3)
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                else
                    std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a3 = argv[3]; // -o flag
            if (a3 != "-o")
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m expected '-o', instead got '%s'\n", a3.c_str());
                else
                    std::fprintf(stderr, "err: expected '-o', instead got '%s'\n", a3.c_str());
                return false;
            }
            if (argc == 4)
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                else
                    std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a4 = argv[4]; // outfile file location
            // start real job
            parser ps(a2, (argc == 6 ? true : false));
            if (!ps.perform_parsing(10))
                return false;
            openutils::sstring a5;
            if (argc == 6)
                a5 = argv[5];
            if (a5.is_null())
            {
                return parser::serialize(ps, a4);
            }
            else if (a5 == "--print-gui-client")
            {
                if (!parser::serialize(ps, a4))
                    return false;
                ps.print_for_gui_client();
                return true;
            }
            else
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m invalid flag passed '%s', try using '--help'\n", a5.c_str());
                else
                    std::fprintf(stderr, "err: invalid flag passed '%s', try using '--help'\n", a5.c_str());
                return false;
            }
        }
        else if (a1 == "--deserialize" || a1 == "-d")
        {
            if (argc == 2)
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                else
                    std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a2 = argv[2]; // file location or print
            if (a2 == "--print" || a2 == "-p")
            {
                if (argc == 3)
                {
                    if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                        std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                    else
                        std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
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
        else if (a1 == "--merge" || a1 == "-m")
        {
            if (argc == 2)
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                else
                    std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a2 = argv[2]; // config file location
            if (argc == 3)
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                else
                    std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a3 = argv[3]; // -o flag
            if (a3 != "-o")
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m expected '-o', instead got '%s'\n", a3.c_str());
                else
                    std::fprintf(stderr, "err: expected '-o', instead got '%s'\n", a3.c_str());
                return false;
            }
            if (argc == 4)
            {
                if (colorize::should_colorize(colorize::STDPTR::STANDARD_ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m not enough arguments were passed, try using '--help'\n");
                else
                    std::fprintf(stderr, "err: not enough arguments were passed, try using '--help'\n");
                return false;
            }
            openutils::sstring a4 = argv[4]; // outfile file location
            // real work
            parser ps(a2);
            if (!ps.perform_parsing(10))
                return false;
            return ps.merge(a4);
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