/**
 * @file import_helper.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./parser.hh"

namespace runcpp
{
    bool parser::import_helper(parser &ps, const openutils::vector_t<std::pair<openutils::sstring, openutils::sstring_lexer_token>> &lexer, const unsigned int &lines_to_read)
    {
        std::size_t j = 0;
        // whitespaces are trimmed
        while (lexer[j].second != openutils::sstring_lexer_token::NULL_END)
        {
            j++; // skip import keyword
            parser::skip_whitespaces_and_tabs(lexer, j);
            if (lexer[j].first == "\"")
            {
                openutils::sstring import_location = parser::validate_quotes(ps, "\"", lexer, j);
                if (import_location.is_null())
                {
                    return false;
                }
                parser::skip_whitespaces_and_tabs(lexer, j);
                parser::skip_comment(lexer, j);
                if (!parser::validate_line_ending(ps, lexer, j))
                {
                    return false;
                }
                if (import_location.is_empty())
                {
                    parser::draw_error(ps.M_curr_location, "file's location", "was (null) or empty", ps.M_curr_line, j - 1, lexer);
                    return false;
                }
                if (!io::file_exists(import_location))
                {
                    parser::draw_error(ps.M_curr_location, "file", import_location.wrap("'") + " not found", ps.M_curr_line, j, lexer);
                    return false;
                }
                // 100% correct syntax and file also exists
                // below block acts like a loop(recursion) for nested import functions
                // by using above way
                // 1. we first transfer ps.map to temp_parser, so that it can parse target calls from the parent file
                // 2. Now, for performance we are using std::move
                parser temp_parser(import_location, ps.store_target_names);
                temp_parser.M_map.operator=(std::move(ps.M_map));
                if (!temp_parser.perform_parsing(lines_to_read))
                    return false;
                ps.M_map.operator=(std::move(temp_parser.M_map));
                if (ps.store_target_names)
                    ps.M_target_vector.add(std::move(temp_parser.M_target_vector));
                return true;
            }
            else
            {
                parser::draw_error(ps.M_curr_location, "expected", "\"", ps.M_curr_line, j, lexer);
                return false;
            }
        }
        return true;
    }
}