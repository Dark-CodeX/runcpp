/**
 * @file caller.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

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
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m CreateProcess failed and returned error code %lu.\r\n", GetLastError());
            else
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
#else
    caller::caller(const openutils::vector_t<openutils::sstring> &arg)
    {
        if (arg.length() > 1)
        {
            if (arg[0] == "sudo")
            {
                if (colorize::should_colorize(colorize::STDPTR::ERR))
                    std::fprintf(stderr, "\033[1;93mwarning:\033[0m entering superuser mode.\n");
                else
                    std::fprintf(stderr, "warning: entering superuser mode.\n");
            }
        }
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
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::perror("\033[1;91merr:\033[0m fork");
            else
                std::perror("err: fork");
            std::exit(EXIT_FAILURE);
        }
        else if (this->pid == 0)
        {
            execvp(this->args.raw_data()[0], this->args.raw_data());
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::perror("\033[1;91merr:\033[0m execvp");
            else
                std::perror("err: execvp");
            std::exit(EXIT_FAILURE);
        }
        else
        {
            int status;
            waitpid(this->pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS)
            {
                if (colorize::should_colorize(colorize::STDPTR::ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m child process exited with status %d\n", WEXITSTATUS(status));
                else
                    std::fprintf(stderr, "err: child process exited with status %d\n", WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                if (colorize::should_colorize(colorize::STDPTR::ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m child process terminated by signal %d\n", WTERMSIG(status));
                else
                    std::fprintf(stderr, "err: child process terminated by signal %d\n", WTERMSIG(status));
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
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::perror("\033[1;91merr:\033[0m popen");
            else
                std::perror("err: popen");
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
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m child process exited with status %d\n", ext_code);
            else
                std::fprintf(stderr, "err: child process exited with status %d\n", ext_code);
            return openutils::vector_t<openutils::sstring>();
        }
        auto return_value = output.to_argv();
        for (std::size_t i = 0; i < return_value.length(); i++)
        {
            // exclude new line or empty strings
            if (return_value[i] == "\n" || return_value[i] == "\r" || return_value[i] == "\r\n" || return_value[i] == "" || return_value[i].is_null())
                return_value.remove(i);
        }
        return return_value;
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
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m environment variable 'PATH' was not found.\n");
            else
                std::fprintf(stderr, "err: environment variable 'PATH' was not found.\n");
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
        return nullptr;
    }
}