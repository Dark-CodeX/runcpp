#include "./caller.hh"

namespace runcpp
{
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
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
            std::fprintf(stderr, "\033[1;91merr:\033[0m CreateProcess failed and returned error code %lu.\r\n", GetLastError());
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
#else
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
            std::perror("\033[1;91merr:\033[0m fork");
            std::exit(EXIT_FAILURE);
        }
        else if (this->pid == 0)
        {
            execvp(this->args.raw_data()[0], this->args.raw_data());
            std::perror("\033[1;91merr:\033[0m execvp");
            std::exit(EXIT_FAILURE);
        }
        else
        {
            int status;
            waitpid(this->pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS)
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m child process exited with status %d\n", WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m child process terminated by signal %d\n", WTERMSIG(status));
            }
        }
    }

    caller::~caller()
    {
        this->args.erase();
    }
#endif

    openutils::vector_t<openutils::sstring> run_command_popen(const openutils::sstring &cmd)
    {
        FILE *fptr;
#if defined _MSC_VER
        fptr = _popen(cmd.c_str(), "r"); // for msvc support
#else
        fptr = popen(cmd.c_str(), "r");
#endif

        if (!fptr)
        {
            std::perror("\033[1;91merr:\033[0m popen");
            return openutils::vector_t<openutils::sstring>();
        }

        openutils::sstring output;
        char buffer[64] = {};

        while (std::fgets(buffer, 63, fptr) != nullptr)
            output.append(buffer);

        int ext_code;
#if defined _MSC_VER
        ext_code = _pclose(fptr); // for msvc support
#else
        ext_code = pclose(fptr);
#endif

        if (ext_code != EXIT_SUCCESS)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m child process exited with status %d\n", ext_code);
            return openutils::vector_t<openutils::sstring>();
        }

        return output.to_argv();
    }

    openutils::sstring get_command_path_if_exists(const openutils::sstring &executable_name)
    {
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
        std::size_t env_len = GetEnvironmentVariableA("PATH", nullptr, 0); // using windows api, instead of getenv()
        openutils::sstring all_paths('\0', env_len + 1);
        GetEnvironmentVariableA("PATH", all_paths.get(), env_len);
#else
        openutils::sstring all_paths = std::getenv("PATH");
#endif
        if (all_paths.is_null())
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m environment variable 'PATH' was not found.\n");
            return nullptr;
        }
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
        openutils::vector_t<openutils::sstring> paths = all_paths.split(";");
        const char *slash = "\\";
#else
        openutils::vector_t<openutils::sstring> paths = all_paths.split(":");
        const char *slash = "/";
#endif
        for (std::size_t i = 0; i < paths.length(); i++)
        {
            if (paths[i].is_null() || paths[i].is_empty())
                continue;
            openutils::sstring temp_loc;
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
            if (paths[i][paths[i].length() - 1] == '\\')
                temp_loc = paths[i] + executable_name;
            else
                temp_loc = paths[i] + slash + executable_name;
#else
            if (paths[i][paths[i].length() - 1] == '/')
                temp_loc = paths[i] + executable_name;
            else
                temp_loc = paths[i] + slash + executable_name;
#endif
            if (io::file_exists(temp_loc))
                return temp_loc;
        }
        std::fprintf(stderr, "\033[1;91merr:\033[0m dependency '%s' was not found.\n", executable_name.c_str());
        return nullptr;
    }
}