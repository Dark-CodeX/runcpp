/**
 * @file helper_functions.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./parser.hh"

namespace runcpp
{
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