/**
 * @file command_line.hh
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#ifndef RUNCPP_COMMAND_LINE_HH
#define RUNCPP_COMMAND_LINE_HH

#include <openutils/sstring/sstring.hh>
#include <openutils/vector/vector.hh>
#include <ctime>

#include "../caller/caller.hh"
#include "../parser/parser.hh"
#include "../io/io.hh"
#include "../os/os.hh"
#include "../help/help.h"

namespace runcpp
{
    class command_line
    {
    public:
        [[nodiscard]] static bool generate_config_helper();

        [[nodiscard]] static bool run_command(const openutils::sstring &file_loc, const openutils::vector_t<openutils::sstring> &args, bool __print__);

        [[nodiscard]] static bool run_command_des(const openutils::sstring &file_loc, const openutils::vector_t<openutils::sstring> &args, bool __print__);

        [[nodiscard]] static openutils::sstring get_date_time();

        static bool parse_command_line(int argc, const char **argv);

        static bool parse_command_line(const std::size_t &argc, const openutils::sstring *argv);
    };
}

#endif