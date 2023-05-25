/**
 * @file io.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./io.hh"

namespace runcpp
{
    bool io::file_exists(const openutils::sstring &loc)
    {
        if (loc.is_null())
            return false;
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
        struct _stat buffer = {};
        return (_stat(loc.c_str(), &buffer) == 0);

#else
        struct stat buffer = {};
        return (stat(loc.c_str(), &buffer) == 0);
#endif
    }
}