#include "./caller.hh"

namespace runcpp
{
#if defined __linux || defined __linux__ || defined linux || defined __unix || defined __unix__ || defined unix
    caller::caller(const openutils::vector_t<openutils::sstring> &arg)
    {
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
                std::fprintf(stderr, "err: child process exited with status `%d`\n", WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                std::fprintf(stderr, "err: child process terminated by signal `%d`\n", WTERMSIG(status));
            }
        }
    }

    caller::~caller()
    {
        this->args.erase();
    }
#else
// windows code
#endif
}