#include "./caller.hh"

namespace runcpp
{
#if defined __linux || defined __linux__ || defined linux || defined __unix || defined __unix__ || defined unix
    caller::caller(const openutils::vector_t<openutils::sstring> &arg)
    {
        this->args = openutils::vector_t<char *>(arg.length() + 1);
        for (std::size_t i = 0; i < arg.length(); i++)
            this->args.add(const_cast<char *>(arg[i].c_str()));
        this->args.add(nullptr);
    }

    void caller::init()
    {
        this->pid = fork();
        if (this->pid == -1)
        {
            std::perror("err: fork");
            std::exit(EXIT_FAILURE);
        }
        else if (this->pid == 0)
        {
            execvp(this->args.raw_data()[0], this->args.raw_data());
            std::perror("err: execvp");
            std::exit(EXIT_FAILURE);
        }
        else
        {
            int status;
            waitpid(this->pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS)
            {
                std::fprintf(stderr, "err: child process exited with status %d\n", WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                std::fprintf(stderr, "err: child process terminated by signal %d\n", WTERMSIG(status));
            }
        }
    }

    caller::~caller()
    {
        this->args.erase();
    }
#else
    caller::caller(const openutils::vector_t<openutils::sstring> &args)
    {
        for (std::size_t i = 0; i < args.length(); i++)
        {
            this->cmds += args[i] + (i < args.length() - 1 ? " " : "");
        }
    }

    void caller::init()
    {
        ZeroMemory(&this->si, sizeof(this->si));
        this->si.cb = sizeof(this->si);
        ZeroMemory(&this->pi, sizeof(this->pi));

        if (!CreateProcess(nullptr, (LPTSTR)(this->cmds.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &this->si, &this->pi))
        {
            std::fprintf(stderr, "err: CreateProcess failed and returned error code %lu.\r\n", GetLastError());
            std::exit(EXIT_FAILURE);
        }

        WaitForSingleObject(this->pi.hProcess, INFINITE);

        CloseHandle(this->pi.hProcess);
        CloseHandle(this->pi.hThread);
    }

    caller::~caller()
    {
        this->cmds.clear();
    }
#endif
}