/**
 * @file colorize.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./colorize.hh"

namespace runcpp
{
    bool colorize::should_colorize(const colorize::STDPTR &fl)
    {
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
        HANDLE h;
        DWORD m;

        h = GetStdHandle((fl == colorize::STDPTR::STANDARD_OUT ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE));
        return (h != INVALID_HANDLE_VALUE) && (h != NULL) && GetConsoleMode(h, &m);
#else
        char const *t = std::getenv("TERM");
        return t && std::strcmp(t, "dumb") != 0 && isatty((fl == colorize::STDPTR::STANDARD_OUT ? STDOUT_FILENO : STDERR_FILENO));
#endif
    }

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
    void colorize::make_it_colored()
    {
        if (should_colorize(STDPTR::STANDARD_OUT))
        {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            GetConsoleMode(hOut, &dwMode);
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
        if (should_colorize(STDPTR::STANDARD_ERR))
        {
            HANDLE hOut = GetStdHandle(STD_ERROR_HANDLE);
            DWORD dwMode = 0;
            GetConsoleMode(hOut, &dwMode);
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}