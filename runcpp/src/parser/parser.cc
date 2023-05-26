/**
 * @file parser.cc
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

    openutils::vector_t<openutils::sstring> parser::convert_2d_vec_to_1d(const openutils::vector_t<openutils::vector_t<openutils::sstring>> &data)
    {
        openutils::vector_t<openutils::sstring> _ret_val_(data.length()); // TDOD: needs optimization
        for (std::size_t i = 0; i < data.length(); i++)
        {
            for (std::size_t j = 0; j < data[i].length(); j++)
            {
                _ret_val_.add(data[i][j]);
            }
        }
        return _ret_val_;
    }

    bool parser::evaluate_ifs(const openutils::vector_t<openutils::sstring> &lhs, const openutils::vector_t<openutils::sstring> &rhs)
    {
        if (lhs.length() == 1 && rhs.length() == 1)
        {
            // some constant comparssion
            const openutils::sstring &ele_lhs_0 = lhs[0];
            const openutils::sstring &ele_rhs_0 = rhs[0];
            if (ele_lhs_0 == "os")
            {
                return ele_rhs_0 == get_os();
            }
            else if (ele_lhs_0 == "os_arch")
            {
                return ele_rhs_0 == openutils::sstring::to_sstring(get_app_arch());
            }
        }
        return lhs == rhs;
    }

    bool parser::is_used_keyword(const std::size_t &__keyword_hash)
    {
        const std::size_t n = sizeof(used_keywords) / sizeof(used_keywords[0]);
        for (std::size_t i = 0; i < n; i++)
            if (__keyword_hash == used_keywords[i])
                return true;
        return false;
    }

    bool parser::import_helper(parser *ps, const openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> &lexer, const openutils::sstring &file_loc, const std::size_t &c_line, const unsigned int &lines_to_read)
    {
        std::size_t j = 0;
        // whitespaces are trimmed
        while (lexer[j].second() != openutils::lexer_token::NULL_END)
        {
            j++; // skip import keyword
            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                j++; // ignore whitespaces
            if (lexer[j].first() == "\"")
            {
                j++; // skip "
                openutils::sstring import_location;

                while (lexer[j].first() != "\"" && lexer[j].second() != openutils::lexer_token::NULL_END)
                {
                    import_location += lexer[j++].first();
                    if (j == lexer.length() - 1)
                    {
                        parser::draw_error(file_loc, "expected", "\"", c_line, j, lexer);
                        return false;
                    }
                }
                j++; // skip "
                while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                    j++; // ignore whitespaces
                if (lexer[j].first() == "#")
                {
                    while (lexer[j].second() != openutils::lexer_token::NULL_END)
                        j++; // skip/ignore every data after that
                }
                if (j != lexer.length() - 1)
                {
                    parser::draw_error(file_loc, "unexpected token", lexer[j].first().wrap("'"), c_line, j, lexer);
                    return false;
                }
                if (import_location.is_null() || import_location.is_empty())
                {
                    parser::draw_error(file_loc, "file's location", "was (null) or empty", c_line, j, lexer);
                    return false;
                }
                if (!io::file_exists(import_location))
                {
                    parser::draw_error(file_loc, "file", import_location.wrap("'") + " not found", c_line, j, lexer);
                    return false;
                }
                // 100% correct syntax and file also exists
                // below block acts like a loop(recursion) for nested import functions
                // by using above way
                // 1. we first transfer ps->map to temp_parser, so that it can parse target calls from the parent file
                // 2. Now, for performance we are using std::move
                parser temp_parser(import_location);
                temp_parser.M_map.operator=(std::move(ps->M_map));
                if (!temp_parser.perform_parsing(lines_to_read))
                    return false;
                ps->M_map.operator=(std::move(temp_parser.M_map));
                return true;
            }
            else
            {
                parser::draw_error(file_loc, "expected", "\"", c_line, j, lexer);
                return false;
            }
        }
        return true;
    }

    bool parser::helper_parsing(parser *ps, openutils::sstring &lable, openutils::vector_t<openutils::vector_t<openutils::sstring>> &adding_vector, openutils::sstring &loc, const openutils::sstring &content, std::unordered_map<std::size_t, openutils::vector_t<openutils::vector_t<openutils::sstring>>> &parsed_data, std::size_t &curr_line, const unsigned int &lines_to_read)
    {
        openutils::vector_t<openutils::sstring> split = content.split("\n");
        for (std::size_t i_split = 0; i_split < split.length(); i_split++, curr_line++)
        {
            openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> lexer = split[i_split].lexer();
            std::size_t j = 0;
            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                j++; // ignore whitespaces
            while (lexer[j].second() != openutils::lexer_token::NULL_END)
            {
                if (lexer[j].first() == "[")
                {
                    if (ps->M_block != parser::BLOCK_TYPE::NONE_BLOCK) // means this block code config file is under if-else
                    {
                        if (ps->M_block == parser::BLOCK_TYPE::IF_BLOCK && !ps->M_was_if_true)
                            break;
                        if (ps->M_block == parser::BLOCK_TYPE::ELSE_BLOCK && !ps->M_was_else_true)
                            break;
                    }
                    j++; // skip [
                    if (!adding_vector.is_null() && !adding_vector.is_empty() && !lable.is_null() && !lable.is_empty())
                        parsed_data[lable.hash()].operator=(std::move(adding_vector));
                    // adding_vector's data has been moved, no need to erose it
                    lable.clear();

                    while (lexer[j].second() != openutils::lexer_token::NULL_END)
                    {
                        if (lexer[j].second() == openutils::lexer_token::WHITESPACE)
                        {
                            parser::draw_error(loc, "unexpected", "' '", curr_line, j, lexer);
                            return false;
                        }
                        else if (lexer[j].first() == "]") // ends
                            break;
                        else if (j >= lexer.length() - 1)
                        {
                            parser::draw_error(loc, "expected", "']'", curr_line, j, lexer);
                            return false;
                        }
                        lable += lexer[j].first();
                        j++;
                    }
                    // now at this point where were no errors
                    if (parsed_data.contains(lable.hash()))
                    {
                        // avoid duplicate targets
                        parser::draw_error(loc, "target", lable.wrap("'") + " is already defined", curr_line, j - 1, lexer);
                        return false;
                    }
                    if (parser::is_used_keyword(lable.hash())) // true means target is a used name
                    {
                        parser::draw_error(loc, "target", lable.wrap("'") + " is a reserved name, try using another similar name instead", curr_line, j - 1, lexer);
                        return false;
                    }
                    j++; // skip ]
                    if (j >= lexer.length() - 1)
                    {
                        parser::draw_error(loc, "expected", "':'", curr_line, j, lexer);
                        return false;
                    }
                    if (lexer[j].first() != ":")
                    {
                        parser::draw_error(loc, "expected", "':'", curr_line, j, lexer);
                        return false;
                    }
                    j++; // skip :
                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                        j++; // ignore whitespaces
                    if (lexer[j].first() == "#")
                    {
                        while (lexer[j].second() != openutils::lexer_token::NULL_END)
                            j++; // skip/ignore every data after that
                    }
                    if (j != lexer.length() - 1)
                    {
                        parser::draw_error(loc, "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                        return false;
                    }
                }
                else if (lexer[j].first() != "if" && lexer[j].first() != "else" && lexer[j].first() != "endif" && lexer[j].first() != "#" && lexer[j].first() != "import")
                {
                    if (ps->M_block != parser::BLOCK_TYPE::NONE_BLOCK) // means this block code config file is under if-else
                    {
                        if (ps->M_block == parser::BLOCK_TYPE::IF_BLOCK && !ps->M_was_if_true)
                            break;
                        if (ps->M_block == parser::BLOCK_TYPE::ELSE_BLOCK && !ps->M_was_else_true)
                            break;
                    }
                    openutils::sstring var_name;
                    while (lexer[j].second() != openutils::lexer_token::NULL_END)
                    {
                        var_name += lexer[j++].first(); // storing variable name, just for target calling
                        if (lexer[j].second() == openutils::lexer_token::WHITESPACE || lexer[j].first() == "=" || lexer[j].first() == "(")
                            break;
                    }
                    if (var_name != "depends") // managing some special cases
                    {
                        if (lable.is_empty() || lable.is_null())
                        {
                            parser::draw_error(loc, "cannot define a variable without a parent target", "", curr_line, j, lexer);
                            return false;
                        }
                    }
                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                        j++; // ignore whitespaces
                    if (lexer[j].first() == "=")
                    {
                        j++; // skip =
                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                            j++; // ignore whitespaces

                        openutils::vector_t<openutils::sstring> temp_vec;
                        // now check if there is ' or [
                        if (lexer[j].first() == "'")
                        {
                            openutils::sstring temp_cmd;
                            j++; // skip '
                            temp_vec.erase();

                            while (lexer[j].first() != "'" && lexer[j].second() != openutils::lexer_token::NULL_END)
                            {
                                temp_cmd += lexer[j++].first();
                                if (j == lexer.length() - 1)
                                {
                                    parser::draw_error(loc, "expected", "'", curr_line, j, lexer);
                                    return false;
                                }
                            }
                            temp_vec.add(temp_cmd);
                            j++; // skip '
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            if (lexer[j].first() == "#")
                            {
                                while (lexer[j].second() != openutils::lexer_token::NULL_END)
                                    j++; // skip/ignore every data after that
                            }
                            if (j != lexer.length() - 1)
                            {
                                parser::draw_error(loc, "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                return false;
                            }
                        }
                        else if (lexer[j].first() == "[")
                        {
                            j++; // skip [
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            temp_vec.erase();
                            while (lexer[j].first() != "]" && lexer[j].second() != openutils::lexer_token::NULL_END)
                            {
                                openutils::sstring temp_cmd;
                                if (lexer[j].first() == "'")
                                {
                                    j++; // skip '
                                    while (lexer[j].first() != "'" && lexer[j].second() != openutils::lexer_token::NULL_END)
                                    {
                                        temp_cmd += lexer[j++].first();
                                        if (j == lexer.length() - 1)
                                        {
                                            parser::draw_error(loc, "expected", "'", curr_line, j, lexer);
                                            return false;
                                        }
                                    }
                                    temp_vec.add(temp_cmd);
                                    j++; // skip '
                                    if (lexer[j].first() == " ")
                                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                            j++; // ignore whitespaces
                                    if (lexer[j].first() == "]")
                                    {
                                        j++;
                                        break;
                                    }
                                    else if (lexer[j].first() == ",")
                                    {
                                        j++;
                                    }
                                    else
                                    {
                                        parser::draw_error(loc, "expected", "' or ']'", curr_line, j, lexer);
                                        return false;
                                    }
                                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                        j++; // ignore whitespaces
                                }
                                else
                                {
                                    parser::draw_error(loc, "expected", "'", curr_line, j, lexer);
                                    return false;
                                }
                            }
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            if (lexer[j].first() == "#")
                            {
                                while (lexer[j].second() != openutils::lexer_token::NULL_END)
                                    j++; // skip/ignore every data after that
                            }
                            if (j != lexer.length() - 1)
                            {
                                parser::draw_error(loc, "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                return false;
                            }
                        }
                        else
                        {
                            parser::draw_error(loc, "expected", "' or '['", curr_line, j, lexer);
                            return false;
                        }
                        adding_vector.add(std::move(temp_vec));
                    }
                    else if (lexer[j].first() == "(")
                    {
                        if (var_name != "shell" && var_name != "depends" && !parsed_data.contains(var_name.hash()))
                        {
                            parser::draw_error(loc, "target", var_name.wrap("'") + " not found", curr_line, j - 1, lexer);
                            return false;
                        }
                        j++; // skip (
                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                            j++; // ignore whitespaces
                        if (var_name == "depends")
                        {
                            // checks if an app is installed or not
                            openutils::sstring app_name = nullptr;
                            if (lexer[j].first() != "'")
                            {
                                parser::draw_error(loc, "expected", "'", curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip '
                            while (lexer[j].first() != "'" && lexer[j].second() != openutils::lexer_token::NULL_END)
                            {
                                app_name += lexer[j++].first();
                                if (j == lexer.length() - 1)
                                {
                                    parser::draw_error(loc, "expected", "'", curr_line, j, lexer);
                                    return false;
                                }
                            }
                            if (app_name.is_null() || app_name.is_empty())
                            {
                                parser::draw_error(loc, "dependency program name was empty or (null)", "", curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip '
                            if (lexer[j].first() != ")")
                            {
                                parser::draw_error(loc, "expected", "')'", curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip )
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            if (lexer[j].first() == "#")
                            {
                                while (lexer[j].second() != openutils::lexer_token::NULL_END)
                                    j++; // skip/ignore every data after that
                            }
                            if (j != lexer.length() - 1)
                            {
                                parser::draw_error(loc, "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                return false;
                            }
                            // now syntax is correct
                            // now check if app is installed or not
                            // if app is installed print its location, else throw error
                            openutils::sstring app_location = get_command_path_if_exists(app_name);
                            if (app_location.is_null())
                            {
                                // error msg is already printed
                                return false;
                            }
                            std::printf("Found '%s' at \033[1;92m'%s'\033[0m\n", app_name.c_str(), app_location.c_str());
                            break;
                        }
                        openutils::sstring shell_command = nullptr;
                        if (var_name == "shell")
                        {
                            // we got a command to directly run and add its output (stdout) to adding_vector
                            // this feature is helpful when there's a situation like `pkgconf --cflags --libs <name>`
                            if (lexer[j].first() != "'")
                            {
                                parser::draw_error(loc, "expected", "'", curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip '
                            while (lexer[j].first() != "'" && lexer[j].second() != openutils::lexer_token::NULL_END)
                            {
                                shell_command += lexer[j++].first();
                                if (j == lexer.length() - 1)
                                {
                                    parser::draw_error(loc, "expected", "'", curr_line, j, lexer);
                                    return false;
                                }
                            }
                            if (shell_command.is_null() || shell_command.is_empty())
                            {
                                parser::draw_error(loc, "shell command was empty or (null)", "", curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip '
                        }
                        if (lexer[j].first() != ")")
                        {
                            parser::draw_error(loc, "expected", "')'", curr_line, j, lexer);
                            return false;
                        }
                        j++; // skip )
                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                            j++; // ignore whitespaces
                        std::size_t select_index_outer = -1, select_index_inner = -1, pos_sio, pos_sii;
                        if (lexer[j].first() == "[")
                        {
                            j++; // skip [
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            if (lexer[j].second() != openutils::lexer_token::INTEGER)
                            {
                                parser::draw_error(loc, "expected an positive integer", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                return false;
                            }
                            pos_sio = j;
                            if (lexer[j].first().length() > std::numeric_limits<std::size_t>::digits10)
                            {
                                parser::draw_error(loc, "integer overflow", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                return false;
                            }
                            select_index_outer = std::stoul(lexer[j].first().c_str()); // error will never happen because of above if statement
                            j++;                                                       // skip the index
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            if (lexer[j].first() != "]")
                            {
                                parser::draw_error(loc, "expected", "']'", curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip ]
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            if (lexer[j].first() == "[")
                            {
                                j++; // skip [
                                while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                    j++; // ignore whitespaces
                                if (lexer[j].second() != openutils::lexer_token::INTEGER)
                                {
                                    parser::draw_error(loc, "expected an positive integer", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                    return false;
                                }
                                pos_sii = j;
                                if (lexer[j].first().length() > std::numeric_limits<std::size_t>::digits10)
                                {
                                    parser::draw_error(loc, "integer overflow", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                    return false;
                                }
                                select_index_inner = std::stoul(lexer[j].first().c_str()); // error will never happen because of above if statement
                                j++;                                                       // skip the index
                                while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                    j++; // ignore whitespaces
                                if (lexer[j].first() != "]")
                                {
                                    parser::draw_error(loc, "expected", "']'", curr_line, j, lexer);
                                    return false;
                                }
                                j++; // skip ]
                                while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                    j++; // ignore whitespaces
                            }
                            else
                            {
                                if (lexer[j].first() == "#")
                                {
                                    while (lexer[j].second() != openutils::lexer_token::NULL_END)
                                        j++; // skip/ignore every data after that
                                }
                            }
                        }
                        else
                        {
                            if (lexer[j].first() == "#")
                            {
                                while (lexer[j].second() != openutils::lexer_token::NULL_END)
                                    j++; // skip/ignore every data after that
                            }
                        }
                        if (j != lexer.length() - 1)
                        {
                            parser::draw_error(loc, "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                            return false;
                        }
                        if (shell_command.is_null())
                        {
                            // now syntax is 100% correct, now as the target which is going to be called is already parsed so we don't need to parse it again, just append its variable's values to current lable's values serial wise
                            // also, target is 100% present as checked above
                            // now, the addition of getting target's particular value using index
                            // we have to manage both `select_index_outer` and `select_index_inner`
                            const openutils::vector_t<openutils::vector_t<openutils::sstring>> &temp_val = parsed_data.at(var_name.hash());
                            if (select_index_outer != static_cast<std::size_t>(-1))
                            {
                                if (select_index_outer >= temp_val.length())
                                {
                                    parser::draw_error(loc, openutils::sstring("out-of-range max length was ") + openutils::sstring::to_sstring(temp_val.length()), openutils::sstring("but the given index was ") + openutils::sstring::to_sstring(select_index_outer), curr_line, pos_sio, lexer);
                                    return false;
                                }
                                else
                                {
                                    if (select_index_inner != static_cast<std::size_t>(-1))
                                    {
                                        if (select_index_inner >= temp_val[select_index_outer].length())
                                        {
                                            parser::draw_error(loc, openutils::sstring("out-of-range max length was ") + openutils::sstring::to_sstring(temp_val[select_index_outer].length()), openutils::sstring("but the given index was ") + openutils::sstring::to_sstring(select_index_inner), curr_line, pos_sii, lexer);
                                            return false;
                                        }
                                        else
                                        {
                                            // here 1D vector is adding up, hence we don't need to convert it
                                            adding_vector.add({temp_val[select_index_outer][select_index_inner]});
                                        }
                                    }
                                    else
                                    {
                                        // here 1D vector is adding up, hence we don't need to convert it
                                        adding_vector.add({temp_val[select_index_outer]});
                                    }
                                }
                            }
                            else
                            {
                                // we need to convert 2D vector to 1D, if lable name is `all`
                                if (lable != "all")
                                    adding_vector.add(temp_val);
                                else
                                    adding_vector.add(parser::convert_2d_vec_to_1d(temp_val));
                            }
                        }
                        else
                        {
                            openutils::vector_t<openutils::sstring> commands_vector = run_command_popen(shell_command);
                            if (commands_vector.is_null() || commands_vector.is_empty())
                                return false;
                            if (select_index_outer != static_cast<std::size_t>(-1))
                            {
                                if (select_index_outer >= commands_vector.length())
                                {
                                    parser::draw_error(loc, openutils::sstring("out-of-range max length was ") + openutils::sstring::to_sstring(commands_vector.length()), openutils::sstring("but the given index was ") + openutils::sstring::to_sstring(select_index_outer), curr_line, pos_sio, lexer);
                                    return false;
                                }
                                if (select_index_inner != static_cast<std::size_t>(-1))
                                {
                                    parser::draw_error(loc, "cannot use another [], as 'shell' returns 1D array", "", curr_line, pos_sii, lexer);
                                    return false;
                                }
                                adding_vector.add({commands_vector[select_index_outer]});
                            }
                            else
                            {
                                // as commands_vector is 1D vector, we don't need to check for `all` target
                                adding_vector.add(std::move(commands_vector));
                            }
                        }
                    }
                    else
                    {
                        parser::draw_error(loc, "expected", "'=' or '('", curr_line, j, lexer);
                        return false;
                    }
                }
                else if (lexer[j].first() == "if")
                {
                    // here was there too many bools to handle, so to avoid using them by passing every variable we are using pointer to the parser object.
                    // i.e., ps to turn on/off the bools
                    j++; // skip if
                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                        j++; // ignore whitespaces
                    openutils::sstring LHS_var;
                    while (lexer[j].second() != openutils::lexer_token::NULL_END)
                    {
                        LHS_var += lexer[j++].first();
                        if (lexer[j].second() == openutils::lexer_token::WHITESPACE || lexer[j].first() == "=" || lexer[j].first() == "!")
                            break;
                    }
                    if (LHS_var.is_null() || LHS_var.is_empty())
                    {
                        parser::draw_error(loc, "expected", "LHS", curr_line, j, lexer);
                        return false;
                    }
                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                        j++; // ignore whitespaces
                    bool is_equal_equal;
                    if (lexer[j].first() == "=")
                    {
                        j++; // skip =
                        if (lexer[j].first() != "=")
                        {
                            parser::draw_error(loc, "expected", "'='", curr_line, j, lexer);
                            return false;
                        }
                        j++; // skip =
                        is_equal_equal = true;
                    }
                    else if (lexer[j].first() == "!")
                    {
                        j++; // skip !
                        if (lexer[j].first() != "=")
                        {
                            parser::draw_error(loc, "expected", "'='", curr_line, j, lexer);
                            return false;
                        }
                        j++; // skip =
                        is_equal_equal = false;
                    }
                    else
                    {
                        parser::draw_error(loc, "expected", "'==' or `!=`", curr_line, j, lexer);
                        return false;
                    }
                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                        j++; // ignore whitespaces

                    openutils::sstring RHS_var;

                    if (lexer[j].first() == "'")
                    {
                        j++; // skip '
                        while (lexer[j].first() != "'" && lexer[j].second() != openutils::lexer_token::NULL_END)
                        {
                            RHS_var += lexer[j++].first();
                            if (j == lexer.length() - 1)
                            {
                                parser::draw_error(loc, "expected", "'", curr_line, j, lexer);
                                return false;
                            }
                        }
                        j++; // skip '
                    }
                    else
                    {
                        parser::draw_error(loc, "expected", "'", curr_line, j, lexer);
                        return false;
                    }
                    if (RHS_var.is_null() || RHS_var.is_empty())
                    {
                        parser::draw_error(loc, "expected", "RHS", curr_line, j, lexer);
                        return false;
                    }
                    if (lexer[j].first() == "#")
                    {
                        while (lexer[j].second() != openutils::lexer_token::NULL_END)
                            j++; // skip/ignore every data after that
                    }
                    if (j != lexer.length() - 1)
                    {
                        parser::draw_error(loc, "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                        return false;
                    }
                    // now syntax is correct
                    ps->M_was_if_true = (is_equal_equal == true ? parser::evaluate_ifs({LHS_var}, {RHS_var}) : !parser::evaluate_ifs({LHS_var}, {RHS_var}));
                    ps->M_block = parser::BLOCK_TYPE::IF_BLOCK;
                }
                else if (lexer[j].first() == "else")
                {
                    if (ps->M_block == parser::BLOCK_TYPE::NONE_BLOCK)
                    {
                        parser::draw_error(loc, "cannot use 'else' without prior if statement", "", curr_line, j, lexer);
                        return false;
                    }
                    if (ps->M_was_if_true)
                    {
                        // if's condition was true, hence else will NOT execute
                        ps->M_was_else_true = false;
                        j++; // skip else
                        if (lexer[j].first() == "#")
                        {
                            while (lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // skip/ignore every data after that
                        }
                        if (j != lexer.length() - 1)
                        {
                            parser::draw_error(loc, "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                            return false;
                        }
                    }
                    else
                    {
                        j++; // skip else
                        if (lexer[j].first() == "#")
                        {
                            while (lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // skip/ignore every data after that
                        }
                        if (j != lexer.length() - 1)
                        {
                            parser::draw_error(loc, "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                            return false;
                        }
                        ps->M_was_else_true = true;
                    }
                    ps->M_block = parser::BLOCK_TYPE::ELSE_BLOCK;
                }
                else if (lexer[j].first() == "endif")
                {
                    if (ps->M_block != parser::BLOCK_TYPE::NONE_BLOCK)
                    {
                        ps->M_was_if_true = false;
                        ps->M_was_else_true = false;
                        ps->M_block = parser::BLOCK_TYPE::NONE_BLOCK;
                    }
                    else
                    {
                        parser::draw_error(loc, "cannot use 'endif' without prior if statement", "", curr_line, j, lexer);
                        return false;
                    }
                    j++; // skip endif
                    if (lexer[j].first() == "#")
                    {
                        while (lexer[j].second() != openutils::lexer_token::NULL_END)
                            j++; // skip/ignore every data after that
                    }
                    if (j != lexer.length() - 1)
                    {
                        parser::draw_error(loc, "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                        return false;
                    }
                }
                else if (lexer[j].first() == "#")
                {
                    // got a comment at starting of a line now, whole line is ignored
                    break; // skip that line
                }
                else if (lexer[j].first() == "import")
                {
                    if (ps->M_block != parser::BLOCK_TYPE::NONE_BLOCK) // means this block code config file is under if-else
                    {
                        if (ps->M_block == parser::BLOCK_TYPE::IF_BLOCK && !ps->M_was_if_true)
                            break;
                        if (ps->M_block == parser::BLOCK_TYPE::ELSE_BLOCK && !ps->M_was_else_true)
                            break;
                    }
                    if (!adding_vector.is_null() && !adding_vector.is_empty() && !lable.is_null() && !lable.is_empty())
                        parsed_data[lable.hash()].operator=(std::move(adding_vector));
                    if (!parser::import_helper(ps, lexer, loc, curr_line, lines_to_read))
                        return false;
                    // now import file's data has been parsed and stored, so we can skip the whole line at this point
                    break; // also, no errors were occurred
                }
                else
                {
                    parser::draw_error(loc, "expected", "'[', 'if', '<variable>' or 'import'", curr_line, j, lexer);
                    return false;
                }
            }
        }
        return true; // parsed without any errors
    }

    parser::parser(const openutils::sstring &location)
    {
        this->M_curr_location = location;
        this->M_curr_line = 0;

        this->M_was_if_true = false;
        this->M_was_else_true = false;
        this->M_block = parser::BLOCK_TYPE::NONE_BLOCK;
    }

    bool parser::perform_parsing(const unsigned int &max_lines)
    {
        if (this->M_curr_location.is_null() || this->M_curr_location.is_empty() || io::file_exists(this->M_curr_location) == false)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' was not found.\n", (this->M_curr_location.is_empty() ? (this->M_curr_location.is_null() ? "(null)" : this->M_curr_location.c_str()) : this->M_curr_location.c_str()));
            return false;
        }
        openutils::chunkio_lines_reader<char> reader(this->M_curr_location.c_str(), max_lines);

        std::pair<char *&, std::size_t> chunk = reader.read_next();

        openutils::sstring temp_content;
        while (chunk.first)
        {
            if (reader.is_file_binary())
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m given file '%s' was binary, which cannot be parsed, try using '-d' flag.\n", this->M_curr_location.c_str());
                return false;
            }
            temp_content.clear();
            temp_content.get() = chunk.first;         // transferring ownership of c-string
            temp_content.change_length(chunk.second); // changing length
            reader.make_nullptr();                    // we have transferred reader's ownership to temp_content

            if (!this->helper_parsing(this, this->M_lable, this->M_adding_vector, this->M_curr_location, temp_content, this->M_map, this->M_curr_line, max_lines))
                return false;

            chunk = reader.read_next();
        }
        if (!this->M_adding_vector.is_null() && !this->M_adding_vector.is_empty() && !this->M_lable.is_null() && !this->M_lable.is_empty())
            this->M_map[this->M_lable.hash()].operator=(std::move(this->M_adding_vector));
        if (this->M_block != parser::BLOCK_TYPE::NONE_BLOCK)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m expected 'endif' at EOF\n");
            return false;
        }
        // at this point we can erase useless memory
        this->clear_memory();
        return true;
    }

    openutils::vector_t<openutils::sstring> parser::generate_command(const openutils::sstring &__key) const
    {
        openutils::vector_t<openutils::sstring> ret_val;
        std::size_t hash = __key.hash();
        if (this->M_map.contains(hash))
        {
            const openutils::vector_t<openutils::vector_t<openutils::sstring>> &temp_val = this->M_map.at(hash);
            for (std::size_t i = 0; i < temp_val.length(); i++)
            {
                for (std::size_t j = 0; j < temp_val[i].length(); j++)
                {
                    ret_val.add(temp_val[i][j]);
                }
            }
        }
        return ret_val;
    }

    const openutils::vector_t<openutils::vector_t<openutils::sstring>> &parser::generate_commands_all() const
    {
        // its the caller's responsibility to check if `all` target exists or NOT
        return this->M_map.at(openutils::sstring("all").hash());
    }

    bool parser::contains_key(const openutils::sstring &__key) const
    {
        return this->M_map.contains(__key.hash());
    }

    void parser::clear_memory()
    {
        this->M_adding_vector.erase();
        this->M_curr_location.clear();
        this->M_lable.clear();
    }

    void parser::print() const
    {
        for (const auto &[key, val] : this->M_map)
        {
            std::printf("%zu:\n", key);
            for (std::size_t i = 0; i < val.length(); i++)
            {
                std::printf("[");
                for (std::size_t j = 0; j < val[i].length(); j++)
                {
                    std::printf("%s%s", val[i][j].c_str(), (j < val[i].length() - 1 ? ", " : ""));
                }
                std::puts("]");
            }
            std::printf("\n"); // prints new line
        }
    }

    bool parser::serialize(const parser &p, const openutils::sstring &location)
    {
        std::FILE *fptr = std::fopen(location.c_str(), "wb");
        if (!fptr)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m could not open file '%s' for writing.\n", location.c_str());
            return false;
        }

        bool is_app_arch_64 = (get_app_arch() == 64 ? true : false);
        std::fwrite(&is_app_arch_64, sizeof(is_app_arch_64), 1, fptr);

        std::size_t desc_len = 46;
        std::fwrite(&desc_len, sizeof(desc_len), 1, fptr);
        const char *desc = "generated using runcpp, don't change manually";
        std::fwrite(desc, sizeof(char), desc_len, fptr);

        std::size_t umap_len = p.M_map.size();
        std::fwrite(&umap_len, sizeof(umap_len), 1, fptr);

        for (const auto &[key, val] : p.M_map)
        {
            std::size_t __key = key;
            std::fwrite(&__key, sizeof(__key), 1, fptr);

            std::size_t outer_vec_len = val.length();
            std::fwrite(&outer_vec_len, sizeof(outer_vec_len), 1, fptr);

            for (std::size_t i = 0; i < val.length(); i++)
            {
                std::size_t inner_vec_len = val[i].length();
                std::fwrite(&inner_vec_len, sizeof(inner_vec_len), 1, fptr);

                for (std::size_t j = 0; j < val[i].length(); j++)
                {
                    std::size_t str_len = val[i][j].length();
                    std::fwrite(&str_len, sizeof(str_len), 1, fptr);
                    std::fwrite(val[i][j].c_str(), sizeof(char), str_len, fptr);
                }
            }
        }

        std::fclose(fptr);
        return true;
    }

    bool parser::deserialize(parser &p, const openutils::sstring &location)
    {
        std::FILE *fptr = std::fopen(location.c_str(), "rb");
        if (!fptr)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m could not open file '%s' for reading.\n", location.c_str());
            return false;
        }

        bool is_binary_64;
        if (std::fread(&is_binary_64, sizeof(is_binary_64), 1, fptr) != 1)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }

        if (is_binary_64 == true && get_app_arch() == 32) // means app is 32 bit, but bin is 64 bit
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is 64 bit whereas, the app/os is 32 bit.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }
        if (is_binary_64 == false && get_app_arch() == 64) // means app is 64 bit, but bin is 32 bit
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is 32 bit whereas, the app/os is 64 bit.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }

        std::size_t desc_len;
        if (std::fread(&desc_len, sizeof(desc_len), 1, fptr) != 1)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }
        if (desc_len != 46)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }
        openutils::sstring desc('\0', desc_len);
        std::fread(desc.get(), sizeof(char), desc_len, fptr);
        if (desc != "generated using runcpp, don't change manually")
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }

        std::size_t umap_len;
        if (std::fread(&umap_len, sizeof(umap_len), 1, fptr) != 1)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }

        p.M_map = std::unordered_map<std::size_t, openutils::vector_t<openutils::vector_t<openutils::sstring>>>();
        p.M_map.reserve(umap_len);

        for (std::size_t i = 0; i < umap_len; i++)
        {
            std::size_t __key;
            if (std::fread(&__key, sizeof(__key), 1, fptr) != 1)
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
                std::fclose(fptr);
                return false;
            }
            std::size_t outer_vec_len;
            if (std::fread(&outer_vec_len, sizeof(outer_vec_len), 1, fptr) != 1)
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
                std::fclose(fptr);
                return false;
            }

            openutils::vector_t<openutils::vector_t<openutils::sstring>> outer_vec(outer_vec_len);

            for (std::size_t j = 0; j < outer_vec_len; j++)
            {
                std::size_t inner_vec_len;
                if (std::fread(&inner_vec_len, sizeof(inner_vec_len), 1, fptr) != 1)
                {
                    std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
                    std::fclose(fptr);
                    return false;
                }

                openutils::vector_t<openutils::sstring> inner_vec(inner_vec_len);

                for (std::size_t k = 0; k < inner_vec_len; k++)
                {
                    std::size_t str_len;
                    if (std::fread(&str_len, sizeof(str_len), 1, fptr) != 1)
                    {
                        std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
                        std::fclose(fptr);
                        return false;
                    }

                    openutils::sstring str('\0', str_len);
                    if (std::fread(str.get(), sizeof(char), str_len, fptr) != str_len)
                    {
                        std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is modified file by external agent.\n", location.c_str());
                        std::fclose(fptr);
                        return false;
                    }

                    inner_vec.add(std::move(str));
                }
                outer_vec.add(std::move(inner_vec));
            }
            p.M_map[__key].operator=(std::move(outer_vec));
        }

        std::fclose(fptr);
        return true;
    }
}