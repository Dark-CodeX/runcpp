/**
 * @file colorize.hh
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#ifndef RUNCPP_COLORIZE_HH
#define RUNCPP_COLORIZE_HH

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace runcpp
{
    class colorize
    {
    public:
        enum class STDPTR : unsigned char
        {
            STANDARD_OUT,
            STANDARD_ERR
        };
        [[nodiscard]] static bool should_colorize(const STDPTR &fl);
    };
}

#endif