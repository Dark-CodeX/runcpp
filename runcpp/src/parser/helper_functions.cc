/**
 * @file helper_functions.cc
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

    void parser::skip_whitespaces_and_tabs(const openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> &lexer, std::size_t &j)
    {
        while ((lexer[j].second() == openutils::lexer_token::WHITESPACE || lexer[j].first() == "\t") && lexer[j].second() != openutils::lexer_token::NULL_END)
            j++; // ignore whitespaces
    }

    void parser::skip_comment(const openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> &lexer, std::size_t &j)
    {
        if (lexer[j].first() == "#")
        {
            while (lexer[j].second() != openutils::lexer_token::NULL_END)
                j++; // skip/ignore every data after that
        }
    }

    bool parser::validate_line_ending(const parser &ps, const openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> &lexer, std::size_t &j)
    {
        if (lexer[j].first() == "\r")
        {
            // added support for CRLF files
            j++;
        }
        if (j != lexer.length() - 1)
        {
            parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
            return false;
        }
        return true;
    }
}