/**
 * @file io.hh
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#ifndef RUNCPP_IO_HH
#define RUNCPP_IO_HH

#include <openutils/sstring/sstring.hh>
#include <sys/stat.h>

namespace runcpp
{
    class io
    {
    public:
        /**
         * @return true if file exists;
         * @return false if file DOES NOT exists
         */
        [[nodiscard]] static bool file_exists(const openutils::sstring &loc);

        /**
         * @return true if file exists;
         * @return false if file DOES NOT exists
         */
        [[nodiscard]] static bool directory_exists(const openutils::sstring &loc);
    };
}
#endif
