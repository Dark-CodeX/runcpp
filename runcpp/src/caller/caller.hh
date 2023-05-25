/**
 * @file caller.hh
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#ifndef RUNCPP_CALLER_HH
#define RUNCPP_CALLER_HH

#include <openutils/vector/vector.hh>
#include <openutils/sstring/sstring.hh>
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
#include <windows.h>
#else
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "../io/io.hh"

namespace runcpp
{
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
    class caller
    {
    private:
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        openutils::sstring cmds;

    public:
        caller(const openutils::vector_t<openutils::sstring> &args);
        void init();
        ~caller();
    };
#else
    class caller
    {
    private:
        openutils::vector_t<char *> args;
        pid_t pid;

    public:
        caller(const openutils::vector_t<openutils::sstring> &arg);
        void init();
        ~caller();
    };
#endif

    [[nodiscard]] openutils::vector_t<openutils::sstring> run_command_popen(const openutils::sstring &cmd);

    [[nodiscard]] openutils::sstring get_command_path_if_exists(const openutils::sstring &executable_name);
}

#endif
