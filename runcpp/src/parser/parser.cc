/**
 * @file parser.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./parser.hh"

namespace runcpp
{
    bool parser::helper_parsing(parser &ps, const openutils::sstring &content, const unsigned int &lines_to_read)
    {
        openutils::vector_t<openutils::sstring> split = content.split("\n");
        for (std::size_t i_split = 0; i_split < split.length(); i_split++, ps.M_curr_line++)
        {
            openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> lexer = split[i_split].lexer();
            std::size_t j = 0;
            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                j++; // ignore whitespaces
            while (lexer[j].second() != openutils::lexer_token::NULL_END)
            {
                if (lexer[j].first() == "[")
                {
                    if (ps.M_block != parser::BLOCK_TYPE::NONE_BLOCK) // means this block code config file is under if-else
                    {
                        if (ps.M_block == parser::BLOCK_TYPE::IF_BLOCK && !ps.M_was_if_true)
                            break;
                        if (ps.M_block == parser::BLOCK_TYPE::ELSE_BLOCK && !ps.M_was_else_true)
                            break;
                    }
                    j++; // skip [
                    if (!ps.M_adding_vector.is_null() && !ps.M_adding_vector.is_empty() && !ps.M_lable.is_null() && !ps.M_lable.is_empty())
                    {
                        if (!ps.M_lable.is_digit())
                            ps.M_map[ps.M_lable.hash()].operator=(std::move(ps.M_adding_vector));
                        else
                        {
                            // treating all digit lable as hash itself
                            std::size_t hash_temp = std::stoul(ps.M_lable.c_str());
                            ps.M_map[hash_temp].operator=(std::move(ps.M_adding_vector));
                        }
                    }
                    // ps.M_adding_vector's data has been moved, no need to erose it
                    ps.M_lable.clear();

                    while (lexer[j].second() != openutils::lexer_token::NULL_END)
                    {
                        if (lexer[j].second() == openutils::lexer_token::WHITESPACE)
                        {
                            parser::draw_error(ps.M_curr_location, "unexpected", "' '", ps.M_curr_line, j, lexer);
                            return false;
                        }
                        else if (lexer[j].first() == "]") // ends
                            break;
                        else if (j >= lexer.length() - 1)
                        {
                            parser::draw_error(ps.M_curr_location, "expected", "']'", ps.M_curr_line, j, lexer);
                            return false;
                        }
                        ps.M_lable += lexer[j].first();
                        j++;
                    }
                    // now at this point where were no errors
                    if (ps.M_map.contains(ps.M_lable.hash()))
                    {
                        // avoid duplicate targets
                        parser::draw_error(ps.M_curr_location, "target", ps.M_lable.wrap("'") + " is already defined", ps.M_curr_line, j - 1, lexer);
                        return false;
                    }
                    if (parser::is_used_keyword(ps.M_lable.hash())) // true means target is a used name
                    {
                        parser::draw_error(ps.M_curr_location, "target", ps.M_lable.wrap("'") + " is a reserved name, try using another similar name instead", ps.M_curr_line, j - 1, lexer);
                        return false;
                    }
                    j++; // skip ]
                    if (j >= lexer.length() - 1)
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "':'", ps.M_curr_line, j, lexer);
                        return false;
                    }
                    if (lexer[j].first() != ":")
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "':'", ps.M_curr_line, j, lexer);
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
                        parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
                        return false;
                    }
                }
                else if (lexer[j].first() != "if" && lexer[j].first() != "else" && lexer[j].first() != "endif" && lexer[j].first() != "#" && lexer[j].first() != "import")
                {
                    if (ps.M_block != parser::BLOCK_TYPE::NONE_BLOCK) // means this block code config file is under if-else
                    {
                        if (ps.M_block == parser::BLOCK_TYPE::IF_BLOCK && !ps.M_was_if_true)
                            break;
                        if (ps.M_block == parser::BLOCK_TYPE::ELSE_BLOCK && !ps.M_was_else_true)
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
                        if (ps.M_lable.is_empty() || ps.M_lable.is_null())
                        {
                            parser::draw_error(ps.M_curr_location, "cannot define a variable without a parent target", "", ps.M_curr_line, j, lexer);
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
                                    parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
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
                                parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
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
                                            parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
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
                                        parser::draw_error(ps.M_curr_location, "expected", "' or ']'", ps.M_curr_line, j, lexer);
                                        return false;
                                    }
                                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                        j++; // ignore whitespaces
                                }
                                else
                                {
                                    parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
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
                                parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
                                return false;
                            }
                        }
                        else
                        {
                            parser::draw_error(ps.M_curr_location, "expected", "' or '['", ps.M_curr_line, j, lexer);
                            return false;
                        }
                        ps.M_adding_vector.add(std::move(temp_vec));
                    }
                    else if (lexer[j].first() == "(")
                    {
                        if (var_name != "shell" && var_name != "depends" && !ps.M_map.contains(var_name.hash()))
                        {
                            parser::draw_error(ps.M_curr_location, "target", var_name.wrap("'") + " not found", ps.M_curr_line, j - 1, lexer);
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
                                parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip '
                            while (lexer[j].first() != "'" && lexer[j].second() != openutils::lexer_token::NULL_END)
                            {
                                app_name += lexer[j++].first();
                                if (j == lexer.length() - 1)
                                {
                                    parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
                                    return false;
                                }
                            }
                            if (app_name.is_null() || app_name.is_empty())
                            {
                                parser::draw_error(ps.M_curr_location, "dependency program name was empty or (null)", "", ps.M_curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip '
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            if (lexer[j].first() != ")")
                            {
                                parser::draw_error(ps.M_curr_location, "expected", "')'", ps.M_curr_line, j, lexer);
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
                                parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
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
                            // we got a command to directly run and add its output (stdout) to ps.M_adding_vector
                            // this feature is helpful when there's a situation like `pkgconf --cflags --libs <name>`
                            if (lexer[j].first() != "'")
                            {
                                parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip '
                            while (lexer[j].first() != "'" && lexer[j].second() != openutils::lexer_token::NULL_END)
                            {
                                shell_command += lexer[j++].first();
                                if (j == lexer.length() - 1)
                                {
                                    parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
                                    return false;
                                }
                            }
                            if (shell_command.is_null() || shell_command.is_empty())
                            {
                                parser::draw_error(ps.M_curr_location, "shell command was empty or (null)", "", ps.M_curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip '
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                        }
                        if (lexer[j].first() != ")")
                        {
                            parser::draw_error(ps.M_curr_location, "expected", "')'", ps.M_curr_line, j, lexer);
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
                                parser::draw_error(ps.M_curr_location, "expected an positive integer", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
                                return false;
                            }
                            pos_sio = j;
                            if (lexer[j].first().length() > std::numeric_limits<std::size_t>::digits10)
                            {
                                parser::draw_error(ps.M_curr_location, "integer overflow", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
                                return false;
                            }
                            select_index_outer = std::stoul(lexer[j].first().c_str()); // error will never happen because of above if statement
                            j++;                                                       // skip the index
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            if (lexer[j].first() != "]")
                            {
                                parser::draw_error(ps.M_curr_location, "expected", "']'", ps.M_curr_line, j, lexer);
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
                                    parser::draw_error(ps.M_curr_location, "expected an positive integer", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
                                    return false;
                                }
                                pos_sii = j;
                                if (lexer[j].first().length() > std::numeric_limits<std::size_t>::digits10)
                                {
                                    parser::draw_error(ps.M_curr_location, "integer overflow", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
                                    return false;
                                }
                                select_index_inner = std::stoul(lexer[j].first().c_str()); // error will never happen because of above if statement
                                j++;                                                       // skip the index
                                while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                    j++; // ignore whitespaces
                                if (lexer[j].first() != "]")
                                {
                                    parser::draw_error(ps.M_curr_location, "expected", "']'", ps.M_curr_line, j, lexer);
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
                            parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
                            return false;
                        }
                        if (shell_command.is_null())
                        {
                            // now syntax is 100% correct, now as the target which is going to be called is already parsed so we don't need to parse it again, just append its variable's values to current lable's values serial wise
                            // also, target is 100% present as checked above
                            // now, the addition of getting target's particular value using index
                            // we have to manage both `select_index_outer` and `select_index_inner`
                            const openutils::vector_t<openutils::vector_t<openutils::sstring>> &temp_val = ps.M_map.at(var_name.hash());
                            if (select_index_outer != static_cast<std::size_t>(-1))
                            {
                                if (select_index_outer >= temp_val.length())
                                {
                                    parser::draw_error(ps.M_curr_location, openutils::sstring("out-of-range max length was ") + openutils::sstring::to_sstring(temp_val.length()), openutils::sstring("but the given index was ") + openutils::sstring::to_sstring(select_index_outer), ps.M_curr_line, pos_sio, lexer);
                                    return false;
                                }
                                else
                                {
                                    if (select_index_inner != static_cast<std::size_t>(-1))
                                    {
                                        if (select_index_inner >= temp_val[select_index_outer].length())
                                        {
                                            parser::draw_error(ps.M_curr_location, openutils::sstring("out-of-range max length was ") + openutils::sstring::to_sstring(temp_val[select_index_outer].length()), openutils::sstring("but the given index was ") + openutils::sstring::to_sstring(select_index_inner), ps.M_curr_line, pos_sii, lexer);
                                            return false;
                                        }
                                        else
                                        {
                                            // here 1D vector is adding up, hence we don't need to convert it
                                            ps.M_adding_vector.add({temp_val[select_index_outer][select_index_inner]});
                                        }
                                    }
                                    else
                                    {
                                        // here 1D vector is adding up, hence we don't need to convert it
                                        ps.M_adding_vector.add({temp_val[select_index_outer]});
                                    }
                                }
                            }
                            else
                            {
                                // we need to convert 2D vector to 1D, if lable name is `all`
                                if (ps.M_lable != "all")
                                    ps.M_adding_vector.add(temp_val);
                                else
                                    ps.M_adding_vector.add(parser::convert_2d_vec_to_1d(temp_val));
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
                                    parser::draw_error(ps.M_curr_location, openutils::sstring("out-of-range max length was ") + openutils::sstring::to_sstring(commands_vector.length()), openutils::sstring("but the given index was ") + openutils::sstring::to_sstring(select_index_outer), ps.M_curr_line, pos_sio, lexer);
                                    return false;
                                }
                                if (select_index_inner != static_cast<std::size_t>(-1))
                                {
                                    parser::draw_error(ps.M_curr_location, "cannot use another [], as 'shell' returns 1D array", "", ps.M_curr_line, pos_sii, lexer);
                                    return false;
                                }
                                ps.M_adding_vector.add({commands_vector[select_index_outer]});
                            }
                            else
                            {
                                // as commands_vector is 1D vector, we don't need to check for `all` target
                                ps.M_adding_vector.add(std::move(commands_vector));
                            }
                        }
                    }
                    else
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "'=' or '('", ps.M_curr_line, j, lexer);
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
                        parser::draw_error(ps.M_curr_location, "expected", "LHS", ps.M_curr_line, j, lexer);
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
                            parser::draw_error(ps.M_curr_location, "expected", "'='", ps.M_curr_line, j, lexer);
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
                            parser::draw_error(ps.M_curr_location, "expected", "'='", ps.M_curr_line, j, lexer);
                            return false;
                        }
                        j++; // skip =
                        is_equal_equal = false;
                    }
                    else
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "'==' or `!=`", ps.M_curr_line, j, lexer);
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
                                parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
                                return false;
                            }
                        }
                        j++; // skip '
                    }
                    else
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
                        return false;
                    }
                    if (RHS_var.is_null() || RHS_var.is_empty())
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "RHS", ps.M_curr_line, j, lexer);
                        return false;
                    }
                    if (lexer[j].first() == "#")
                    {
                        while (lexer[j].second() != openutils::lexer_token::NULL_END)
                            j++; // skip/ignore every data after that
                    }
                    if (j != lexer.length() - 1)
                    {
                        parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
                        return false;
                    }
                    // now syntax is correct
                    ps.M_was_if_true = (is_equal_equal == true ? parser::evaluate_ifs({LHS_var}, {RHS_var}) : !parser::evaluate_ifs({LHS_var}, {RHS_var}));
                    ps.M_block = parser::BLOCK_TYPE::IF_BLOCK;
                }
                else if (lexer[j].first() == "else")
                {
                    if (ps.M_block == parser::BLOCK_TYPE::NONE_BLOCK)
                    {
                        parser::draw_error(ps.M_curr_location, "cannot use 'else' without prior if statement", "", ps.M_curr_line, j, lexer);
                        return false;
                    }
                    if (ps.M_was_if_true)
                    {
                        // if's condition was true, hence else will NOT execute
                        ps.M_was_else_true = false;
                        j++; // skip else
                        if (lexer[j].first() == "#")
                        {
                            while (lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // skip/ignore every data after that
                        }
                        if (j != lexer.length() - 1)
                        {
                            parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
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
                            parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
                            return false;
                        }
                        ps.M_was_else_true = true;
                    }
                    ps.M_block = parser::BLOCK_TYPE::ELSE_BLOCK;
                }
                else if (lexer[j].first() == "endif")
                {
                    if (ps.M_block != parser::BLOCK_TYPE::NONE_BLOCK)
                    {
                        ps.M_was_if_true = false;
                        ps.M_was_else_true = false;
                        ps.M_block = parser::BLOCK_TYPE::NONE_BLOCK;
                    }
                    else
                    {
                        parser::draw_error(ps.M_curr_location, "cannot use 'endif' without prior if statement", "", ps.M_curr_line, j, lexer);
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
                        parser::draw_error(ps.M_curr_location, "unexpected token", lexer[j].first().wrap("'"), ps.M_curr_line, j, lexer);
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
                    if (ps.M_block != parser::BLOCK_TYPE::NONE_BLOCK) // means this block code config file is under if-else
                    {
                        if (ps.M_block == parser::BLOCK_TYPE::IF_BLOCK && !ps.M_was_if_true)
                            break;
                        if (ps.M_block == parser::BLOCK_TYPE::ELSE_BLOCK && !ps.M_was_else_true)
                            break;
                    }
                    if (!ps.M_adding_vector.is_null() && !ps.M_adding_vector.is_empty() && !ps.M_lable.is_null() && !ps.M_lable.is_empty())
                    {
                        if (!ps.M_lable.is_digit())
                            ps.M_map[ps.M_lable.hash()].operator=(std::move(ps.M_adding_vector));
                        else
                        {
                            // treating all digit lable as hash itself
                            std::size_t hash_temp = std::stoul(ps.M_lable.c_str());
                            ps.M_map[hash_temp].operator=(std::move(ps.M_adding_vector));
                        }
                    }
                    if (!parser::import_helper(ps, lexer, lines_to_read))
                        return false;
                    // now import file's data has been parsed and stored, so we can skip the whole line at this point
                    break; // also, no errors were occurred
                }
                else
                {
                    parser::draw_error(ps.M_curr_location, "expected", "'[', 'if', '<variable>' or 'import'", ps.M_curr_line, j, lexer);
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

            if (!this->helper_parsing(*this, temp_content, max_lines))
                return false;
            this->M_curr_line--;
            chunk = reader.read_next();
        }
        if (!this->M_adding_vector.is_null() && !this->M_adding_vector.is_empty() && !this->M_lable.is_null() && !this->M_lable.is_empty())
        {
            if (!this->M_lable.is_digit())
                this->M_map[this->M_lable.hash()].operator=(std::move(this->M_adding_vector));
            else
            {
                // treating all digit lable as hash itself
                std::size_t hash_temp = std::stoul(this->M_lable.c_str());
                this->M_map[hash_temp].operator=(std::move(this->M_adding_vector));
            }
        }
        if (this->M_block != parser::BLOCK_TYPE::NONE_BLOCK)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m expected 'endif' at EOF\n");
            return false;
        }
        // at this point we can erase useless memory
        this->clear_memory();
        return true;
    }
}