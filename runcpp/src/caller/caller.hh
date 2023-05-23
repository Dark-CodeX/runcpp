#ifndef CALLER_HH
#define CALLER_HH

#include <openutils/vector/vector.hh>
#include <openutils/sstring/sstring.hh>
#if defined __linux || defined __linux__ || defined linux || defined __unix || defined __unix__ || defined unix
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

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
}

#endif
