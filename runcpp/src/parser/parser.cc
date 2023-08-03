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
            openutils::vector_t<std::pair<openutils::sstring, openutils::sstring_lexer_token>> lexer = split[i_split].lexer();
            std::size_t j = 0;
            parser::skip_whitespaces_and_tabs(lexer, j);
            while (lexer[j].second != openutils::sstring_lexer_token::NULL_END)
            {
                if (lexer[j].first == "\r")
                {
                    if (!parser::validate_line_ending(ps, lexer, j))
                    {
                        return false;
                    }
                }
                else if (lexer[j].first == "[")
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
                        {
                            ps.M_map[ps.M_lable.hash()].operator=(std::move(ps.M_adding_vector));
                            if (ps.store_target_names)
                                ps.M_target_vector.add(ps.M_lable);
                        }
                        else
                        {
                            // treating all digit lable as hash itself
                            if (ps.store_target_names)
                            {
                                if (colorize::should_colorize(colorize::STDPTR::ERR))
                                    std::fprintf(stderr, "\033[1;91merr:\033[0m cannot use hashed target name '%s' with '--print-gui-client' flag.\n", ps.M_lable.c_str());
                                else
                                    std::fprintf(stderr, "err: cannot use hashed target name '%s' with '--print-gui-client' flag.\n", ps.M_lable.c_str());
                                return false;
                            }
                            if (ps.M_lable.length() > std::numeric_limits<std::size_t>::digits10)
                            {
                                if (colorize::should_colorize(colorize::STDPTR::ERR))
                                    std::fprintf(stderr, "\033[1;91merr:\033[0m integer overflow by '%s'\n", ps.M_lable.c_str());
                                else
                                    std::fprintf(stderr, "err: integer overflow by '%s'\n", ps.M_lable.c_str());
                                return false;
                            }
                            std::size_t hash_temp = std::stoul(ps.M_lable.c_str());
                            ps.M_map[hash_temp].operator=(std::move(ps.M_adding_vector));
                        }
                    }
                    // ps.M_adding_vector's data has been moved, no need to erose it
                    ps.M_lable.clear();

                    while (lexer[j].second != openutils::sstring_lexer_token::NULL_END)
                    {
                        if (lexer[j].second == openutils::sstring_lexer_token::WHITESPACE)
                        {
                            parser::draw_error(ps.M_curr_location, "unexpected", "' '", ps.M_curr_line, j, lexer);
                            return false;
                        }
                        else if (lexer[j].first == "]") // ends
                            break;
                        else if (j >= lexer.length() - 1)
                        {
                            parser::draw_error(ps.M_curr_location, "expected", "']'", ps.M_curr_line, j, lexer);
                            return false;
                        }
                        ps.M_lable += lexer[j].first;
                        j++;
                    }
                    // now at this point where were no errors
                    if (ps.M_lable.is_digit())
                    {
                        if (ps.M_lable.length() > std::numeric_limits<std::size_t>::digits10)
                        {
                            parser::draw_error(ps.M_curr_location, "integer overflow", lexer[j].first.wrap("'"), ps.M_curr_line, j - 1, lexer);
                            return false;
                        }
                        std::size_t hash_temp = std::stoul(ps.M_lable.c_str());
                        if (ps.M_map.contains(hash_temp))
                        {
                            // avoid duplicate targets
                            parser::draw_error(ps.M_curr_location, "target with hash", ps.M_lable.wrap("'") + " is already defined", ps.M_curr_line, j - 1, lexer);
                            return false;
                        }
                    }
                    else
                    {
                        if (ps.M_map.contains(ps.M_lable.hash()))
                        {
                            // avoid duplicate targets
                            parser::draw_error(ps.M_curr_location, "target", ps.M_lable.wrap("'") + " is already defined", ps.M_curr_line, j - 1, lexer);
                            return false;
                        }
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
                    if (lexer[j].first != ":")
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "':'", ps.M_curr_line, j, lexer);
                        return false;
                    }
                    j++; // skip :
                    parser::skip_whitespaces_and_tabs(lexer, j);
                    parser::skip_comment(lexer, j);
                    if (!parser::validate_line_ending(ps, lexer, j))
                    {
                        return false;
                    }
                }
                else if (lexer[j].first != "if" && lexer[j].first != "else" && lexer[j].first != "endif" && lexer[j].first != "#" && lexer[j].first != "import")
                {
                    if (ps.M_block != parser::BLOCK_TYPE::NONE_BLOCK) // means this block code config file is under if-else
                    {
                        if (ps.M_block == parser::BLOCK_TYPE::IF_BLOCK && !ps.M_was_if_true)
                            break;
                        if (ps.M_block == parser::BLOCK_TYPE::ELSE_BLOCK && !ps.M_was_else_true)
                            break;
                    }
                    openutils::sstring var_name;
                    while (lexer[j].second != openutils::sstring_lexer_token::NULL_END)
                    {
                        var_name += lexer[j++].first; // storing variable name, just for target calling
                        if (lexer[j].second == openutils::sstring_lexer_token::WHITESPACE || lexer[j].first == "=" || lexer[j].first == "(")
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
                    parser::skip_whitespaces_and_tabs(lexer, j);
                    if (lexer[j].first == "=")
                    {
                        j++; // skip =
                        parser::skip_whitespaces_and_tabs(lexer, j);

                        openutils::vector_t<openutils::sstring> temp_vec;
                        // now check if there is ' or [
                        if (lexer[j].first == "'")
                        {
                            temp_vec.erase();
                            openutils::sstring temp_cmd = parser::validate_quotes(ps, "'", lexer, j);
                            if (temp_cmd.is_null())
                            {
                                return false;
                            }
                            temp_vec.add(temp_cmd);
                            parser::skip_whitespaces_and_tabs(lexer, j);
                            parser::skip_comment(lexer, j);
                            if (!parser::validate_line_ending(ps, lexer, j))
                            {
                                return false;
                            }
                        }
                        else if (lexer[j].first == "[")
                        {
                            temp_vec.erase();
                            temp_vec = std::move(parser::validate_array(ps, "'", lexer, j));
                            if (temp_vec.is_null())
                            {
                                return false;
                            }
                            parser::skip_whitespaces_and_tabs(lexer, j);
                            parser::skip_comment(lexer, j);
                            if (!parser::validate_line_ending(ps, lexer, j))
                            {
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
                    else if (lexer[j].first == "(")
                    {
                        if (var_name != "shell" && var_name != "depends" && !ps.M_map.contains(var_name.hash()))
                        {
                            parser::draw_error(ps.M_curr_location, "target", var_name.wrap("'") + " not found", ps.M_curr_line, j - 1, lexer);
                            return false;
                        }
                        j++; // skip (
                        parser::skip_whitespaces_and_tabs(lexer, j);
                        if (var_name == "depends")
                        {
                            // checks if an app is installed or not
                            if (lexer[j].first != "'")
                            {
                                parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
                                return false;
                            }
                            openutils::sstring dependent = parser::validate_quotes(ps, "'", lexer, j);
                            if (dependent.is_null())
                            {
                                return false;
                            }
                            if (dependent.is_empty())
                            {
                                parser::draw_error(ps.M_curr_location, "dependency program name or path was empty or (null)", "", ps.M_curr_line, j - 1, lexer);
                                return false;
                            }
                            parser::skip_whitespaces_and_tabs(lexer, j);
                            if (lexer[j].first != ")")
                            {
                                parser::draw_error(ps.M_curr_location, "expected", "')'", ps.M_curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip )
                            parser::skip_whitespaces_and_tabs(lexer, j);
                            parser::skip_comment(lexer, j);
                            if (!parser::validate_line_ending(ps, lexer, j))
                            {
                                return false;
                            }
                            // now syntax is correct
                            // now check if app, file, directory is installed or not
                            // if app is installed print its location, else throw error
                            openutils::sstring app_location = get_command_path_if_exists(dependent);
                            if (app_location.is_null())
                            {
                                if (io::directory_exists(dependent))
                                {
                                    if (colorize::should_colorize(colorize::STDPTR::OUT))
                                        std::printf("Found dependency directory at \033[1;92m'%s'\033[0m\n", dependent.c_str());
                                    else
                                        std::printf("Found dependency directory at '%s'\n", dependent.c_str());
                                    break;
                                }
                                else
                                {
                                    if (io::file_exists(dependent))
                                    {
                                        if (colorize::should_colorize(colorize::STDPTR::OUT))
                                            std::printf("Found dependency file at \033[1;92m'%s'\033[0m\n", dependent.c_str());
                                        else
                                            std::printf("Found dependency file at '%s'\n", dependent.c_str());
                                        break;
                                    }
                                    else
                                    {
                                        if (colorize::should_colorize(colorize::STDPTR::ERR))
                                            std::fprintf(stderr, "\033[1;91merr:\033[0m dependency '%s' was not found.\n", dependent.c_str());
                                        else
                                            std::fprintf(stderr, "err: dependency '%s' was not found.\n", dependent.c_str());
                                        return false;
                                    }
                                }
                            }
                            else
                            {
                                if (colorize::should_colorize(colorize::STDPTR::OUT))
                                    std::printf("Found dependency '%s' at \033[1;92m'%s'\033[0m\n", dependent.c_str(), app_location.c_str());
                                else
                                    std::printf("Found dependency '%s' at '%s'\n", dependent.c_str(), app_location.c_str());
                                break;
                            }
                        }
                        openutils::sstring shell_command = nullptr;
                        if (var_name == "shell")
                        {
                            // we got a command to directly run and add its output (stdout) to ps.M_adding_vector
                            // this feature is helpful when there's a situation like `pkgconf --cflags --libs <name>`
                            if (lexer[j].first != "'")
                            {
                                parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
                                return false;
                            }
                            shell_command = std::move(parser::validate_quotes(ps, "'", lexer, j));
                            if (shell_command.is_null())
                            {
                                return false;
                            }
                            if (shell_command.is_empty())
                            {
                                parser::draw_error(ps.M_curr_location, "shell command was empty or (null)", "", ps.M_curr_line, j - 1, lexer);
                                return false;
                            }
                            parser::skip_whitespaces_and_tabs(lexer, j);
                        }
                        if (lexer[j].first != ")")
                        {
                            parser::draw_error(ps.M_curr_location, "expected", "')'", ps.M_curr_line, j, lexer);
                            return false;
                        }
                        j++; // skip )
                        parser::skip_whitespaces_and_tabs(lexer, j);
                        std::size_t select_index_outer = -1, select_index_inner = -1, pos_sio, pos_sii;
                        if (lexer[j].first == "[")
                        {
                            j++; // skip [
                            parser::skip_whitespaces_and_tabs(lexer, j);
                            if (lexer[j].second != openutils::sstring_lexer_token::INTEGER)
                            {
                                parser::draw_error(ps.M_curr_location, "expected an positive integer", lexer[j].first.wrap("'"), ps.M_curr_line, j, lexer);
                                return false;
                            }
                            pos_sio = j;
                            if (lexer[j].first.length() > std::numeric_limits<std::size_t>::digits10)
                            {
                                parser::draw_error(ps.M_curr_location, "integer overflow", lexer[j].first.wrap("'"), ps.M_curr_line, j, lexer);
                                return false;
                            }
                            select_index_outer = std::stoul(lexer[j].first.c_str()); // error will never happen because of above if statement
                            j++;                                                     // skip the index
                            parser::skip_whitespaces_and_tabs(lexer, j);
                            if (lexer[j].first != "]")
                            {
                                parser::draw_error(ps.M_curr_location, "expected", "']'", ps.M_curr_line, j, lexer);
                                return false;
                            }
                            j++; // skip ]
                            parser::skip_whitespaces_and_tabs(lexer, j);
                            if (lexer[j].first == "[")
                            {
                                j++; // skip [
                                parser::skip_whitespaces_and_tabs(lexer, j);
                                if (lexer[j].second != openutils::sstring_lexer_token::INTEGER)
                                {
                                    parser::draw_error(ps.M_curr_location, "expected an positive integer", lexer[j].first.wrap("'"), ps.M_curr_line, j, lexer);
                                    return false;
                                }
                                pos_sii = j;
                                if (lexer[j].first.length() > std::numeric_limits<std::size_t>::digits10)
                                {
                                    parser::draw_error(ps.M_curr_location, "integer overflow", lexer[j].first.wrap("'"), ps.M_curr_line, j, lexer);
                                    return false;
                                }
                                select_index_inner = std::stoul(lexer[j].first.c_str()); // error will never happen because of above if statement
                                j++;                                                     // skip the index
                                parser::skip_whitespaces_and_tabs(lexer, j);
                                if (lexer[j].first != "]")
                                {
                                    parser::draw_error(ps.M_curr_location, "expected", "']'", ps.M_curr_line, j, lexer);
                                    return false;
                                }
                                j++; // skip ]
                                parser::skip_whitespaces_and_tabs(lexer, j);
                            }
                            else
                            {
                                parser::skip_comment(lexer, j);
                            }
                        }
                        else
                        {
                            parser::skip_comment(lexer, j);
                        }
                        if (!parser::validate_line_ending(ps, lexer, j))
                        {
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
                else if (lexer[j].first == "if")
                {
                    // here was there too many bools to handle, so to avoid using them by passing every variable we are using pointer to the parser object.
                    // i.e., ps to turn on/off the bools
                    j++; // skip if
                    parser::skip_whitespaces_and_tabs(lexer, j);
                    openutils::sstring LHS_var;
                    while (lexer[j].second != openutils::sstring_lexer_token::NULL_END)
                    {
                        LHS_var += lexer[j++].first;
                        if (lexer[j].second == openutils::sstring_lexer_token::WHITESPACE || lexer[j].first == "=" || lexer[j].first == "!")
                            break;
                    }
                    if (LHS_var.is_null() || LHS_var.is_empty())
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "LHS", ps.M_curr_line, j, lexer);
                        return false;
                    }
                    parser::skip_whitespaces_and_tabs(lexer, j);
                    bool is_equal_equal;
                    if (lexer[j].first == "=")
                    {
                        j++; // skip =
                        if (lexer[j].first != "=")
                        {
                            parser::draw_error(ps.M_curr_location, "expected", "'='", ps.M_curr_line, j, lexer);
                            return false;
                        }
                        j++; // skip =
                        is_equal_equal = true;
                    }
                    else if (lexer[j].first == "!")
                    {
                        j++; // skip !
                        if (lexer[j].first != "=")
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
                    parser::skip_whitespaces_and_tabs(lexer, j);

                    openutils::sstring RHS_var;

                    if (lexer[j].first == "'")
                    {
                        RHS_var = std::move(parser::validate_quotes(ps, "'", lexer, j));
                        if (RHS_var.is_null())
                        {
                            return false;
                        }
                    }
                    else
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "'", ps.M_curr_line, j, lexer);
                        return false;
                    }
                    if (RHS_var.is_empty())
                    {
                        parser::draw_error(ps.M_curr_location, "expected", "RHS", ps.M_curr_line, j - 1, lexer);
                        return false;
                    }
                    parser::skip_whitespaces_and_tabs(lexer, j);
                    parser::skip_comment(lexer, j);
                    if (!parser::validate_line_ending(ps, lexer, j))
                    {
                        return false;
                    }
                    // now syntax is correct
                    ps.M_was_if_true = (is_equal_equal == true ? parser::evaluate_ifs({LHS_var}, {RHS_var}) : !parser::evaluate_ifs({LHS_var}, {RHS_var}));
                    ps.M_block = parser::BLOCK_TYPE::IF_BLOCK;
                }
                else if (lexer[j].first == "else")
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
                        parser::skip_whitespaces_and_tabs(lexer, j);
                        parser::skip_comment(lexer, j);
                        if (!parser::validate_line_ending(ps, lexer, j))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        j++; // skip else
                        parser::skip_whitespaces_and_tabs(lexer, j);
                        parser::skip_comment(lexer, j);
                        if (!parser::validate_line_ending(ps, lexer, j))
                        {
                            return false;
                        }
                        ps.M_was_else_true = true;
                    }
                    ps.M_block = parser::BLOCK_TYPE::ELSE_BLOCK;
                }
                else if (lexer[j].first == "endif")
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
                    parser::skip_whitespaces_and_tabs(lexer, j);
                    parser::skip_comment(lexer, j);
                    if (!parser::validate_line_ending(ps, lexer, j))
                    {
                        return false;
                    }
                }
                else if (lexer[j].first == "#")
                {
                    // got a comment at starting of a line now, whole line is ignored
                    break; // skip that line
                }
                else if (lexer[j].first == "import")
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
                        {
                            ps.M_map[ps.M_lable.hash()].operator=(std::move(ps.M_adding_vector));
                            if (ps.store_target_names)
                                ps.M_target_vector.add(ps.M_lable);
                        }
                        else
                        {
                            // treating all digit lable as hash itself
                            if (ps.store_target_names)
                            {
                                if (colorize::should_colorize(colorize::STDPTR::ERR))
                                    std::fprintf(stderr, "\033[1;91merr:\033[0m cannot use hashed target name '%s' with '--print-gui-client' flag.\n", ps.M_lable.c_str());
                                else
                                    std::fprintf(stderr, "err: cannot use hashed target name '%s' with '--print-gui-client' flag.\n", ps.M_lable.c_str());
                                return false;
                            }
                            if (ps.M_lable.length() > std::numeric_limits<std::size_t>::digits10)
                            {
                                if (colorize::should_colorize(colorize::STDPTR::ERR))
                                    std::fprintf(stderr, "\033[1;91merr:\033[0m integer overflow by '%s'\n", ps.M_lable.c_str());
                                else
                                    std::fprintf(stderr, "err: integer overflow by '%s'\n", ps.M_lable.c_str());
                                return false;
                            }
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

    parser::parser(const openutils::sstring &location, bool store_target)
    {
        this->M_curr_location = location;
        this->M_curr_line = 0;

        this->M_was_if_true = false;
        this->M_was_else_true = false;
        this->M_block = parser::BLOCK_TYPE::NONE_BLOCK;

        this->store_target_names = store_target;
    }

    bool parser::perform_parsing(const unsigned int &max_lines)
    {
        if (this->M_curr_location.is_empty())
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m empty file location was given\n");
            else
                std::fprintf(stderr, "err: empty file location was given\n");
            return false;
        }

        if (!io::file_exists(this->M_curr_location))
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' was not found: %s\n", this->M_curr_location.c_str(), std::strerror(errno));
            else
                std::fprintf(stderr, "err: file '%s' was not found: %s\n", this->M_curr_location.c_str(), std::strerror(errno));
            return false;
        }

        io_reader reader(max_lines);
        if (!reader.open_file(this->M_curr_location))
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be opened for reading: %s\n", this->M_curr_location.c_str(), std::strerror(errno));
            else
                std::fprintf(stderr, "err: file '%s' could not be opened for reading: %s\n", this->M_curr_location.c_str(), std::strerror(errno));
            return false;
        }

        openutils::sstring &file_lines = reader.read_next();

        while (!file_lines.is_null())
        {
            if (!this->helper_parsing(*this, file_lines, max_lines))
                return false;
            this->M_curr_line--;
            file_lines = reader.read_next();
        }
        if (!this->M_adding_vector.is_null() && !this->M_adding_vector.is_empty() && !this->M_lable.is_null() && !this->M_lable.is_empty())
        {
            if (!this->M_lable.is_digit())
            {
                this->M_map[this->M_lable.hash()].operator=(std::move(this->M_adding_vector));
                if (this->store_target_names)
                    this->M_target_vector.add(this->M_lable);
            }
            else
            {
                // treating all digit lable as hash itself
                if (this->store_target_names)
                {
                    if (colorize::should_colorize(colorize::STDPTR::ERR))
                        std::fprintf(stderr, "\033[1;91merr:\033[0m cannot use hashed target name '%s' with '--print-gui-client' flag.\n", this->M_lable.c_str());
                    else
                        std::fprintf(stderr, "err: cannot use hashed target name '%s' with '--print-gui-client' flag.\n", this->M_lable.c_str());
                    return false;
                }
                if (this->M_lable.length() > std::numeric_limits<std::size_t>::digits10)
                {
                    if (colorize::should_colorize(colorize::STDPTR::ERR))
                        std::fprintf(stderr, "\033[1;91merr:\033[0m integer overflow by '%s'\n", this->M_lable.c_str());
                    else
                        std::fprintf(stderr, "err: integer overflow by '%s'\n", this->M_lable.c_str());
                    return false;
                }
                std::size_t hash_temp = std::stoul(this->M_lable.c_str());
                this->M_map[hash_temp].operator=(std::move(this->M_adding_vector));
            }
        }
        if (this->M_block != parser::BLOCK_TYPE::NONE_BLOCK)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m expected 'endif' at EOF\n");
            else
                std::fprintf(stderr, "err: expected 'endif' at EOF\n");
            return false;
        }
        // at this point we can erase useless memory
        this->clear_memory();
        return true;
    }
}