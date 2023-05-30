/**
 * @file draw_error.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./parser.hh"

namespace runcpp
{
    void parser::draw_error(const openutils::sstring &loc, const openutils::sstring &err_msg, const openutils::sstring &expected, std::size_t line_no, std::size_t curr_lexer, const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::lexer_token>> &lexer)
    {
        std::size_t col = 0;
        for (std::size_t r = 0; r < curr_lexer; r++)
            col += lexer[r].first().length();
        std::fprintf(stderr, "\033[1;97m%s:%zu:%zu:\033[0m \033[1;91merr:\033[0m %s %s\n", loc.c_str(), line_no + 1, col + 1, err_msg.c_str(), expected.c_str());

        for (std::size_t r = 0; r < lexer.length() - 1; r++) // -1 for nullptr
        {
            if (r == curr_lexer && r != lexer.length() - 1)
                std::fprintf(stderr, "\033[1;91m%s\033[0m", lexer[r].first().c_str());
            else
                std::fprintf(stderr, "%s", lexer[r].first().c_str());
        }

        std::fprintf(stderr, "\n%s\033[1;91m^\033[0m\n", (col == 0 ? "" : openutils::sstring('~', col).c_str()));
    }
}