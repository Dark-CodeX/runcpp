/**
 * @file formatter.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#ifndef RUNCPP_FORMAT_FORMATTER_HH
#define RUNCPP_FORMAT_FORMATTER_HH

#include <openutils/sstring/sstring.hh>
#include <openutils/chunkio/chunkio_lines_reader.hh>

#include "../io/io.hh"

namespace runcpp_format
{
    class formatter
    {
    private:                                    // private variables
        openutils::sstring M_location;          // location of the given file
        openutils::sstring M_formatted_content; // for storing the formatted content of the file

    public: // deleted functions
        formatter() = delete;
        formatter(const formatter &) = delete;
        formatter(formatter &&) = delete;
        formatter &operator=(const formatter &) = delete;
        formatter &operator=(formatter &&) = delete;

    private: // private static functions
        [[nodiscard]] bool formatting_helper(const openutils::sstring &content, openutils::sstring &appending_content);

    public: // public functions
        formatter(const openutils::sstring &loc);
        [[nodiscard]] bool do_formatting(const unsigned int &max_lines);
        [[nodiscard]] bool save_or_overwrite();
    };
}

#endif