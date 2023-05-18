#include "./parser.hh"

namespace runcpp
{
    void parser::draw_error(const openutils::sstring &loc, const openutils::sstring &line, const openutils::sstring &err_msg, const openutils::sstring &expected, std::size_t line_no, std::size_t curr_lexer, const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::lexer_token>> &lexer)
    {
        std::size_t col = 0;
        for (std::size_t r = 0; r < curr_lexer; r++)
            col += lexer[r].first().length();
        std::fprintf(stderr, "%s:%zu:%zu: \033[1;91merr:\033[0m %s %s\n", loc.c_str(), line_no + 1, col + 1, err_msg.c_str(), expected.c_str());

        for (std::size_t r = 0; r < lexer.length() - 1; r++) // -1 for nullptr
        {
            if (r == curr_lexer && r != lexer.length() - 1)
                std::fprintf(stderr, "\033[1;91m%s\033[0m", lexer[r].first().c_str());
            else
                std::fprintf(stderr, "%s", lexer[r].first().c_str());
        }

        std::fprintf(stderr, "\n%s\033[1;91m^\033[0m\n", (col == 0 ? "" : openutils::sstring('~', col).c_str()));
    }

    bool parser::import_helper(parser *__parser__, const openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> &lexer, const openutils::sstring &file_loc, const openutils::sstring &curr_line_content, const std::size_t &c_line, const unsigned int &lines_to_read)
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
                        parser::draw_error(file_loc, curr_line_content, "expected", "\"", c_line, j, lexer);
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
                    parser::draw_error(file_loc, curr_line_content, "unexpected token", lexer[j].first().wrap("'"), c_line, j, lexer);
                    return false;
                }
                if (import_location.is_null() || import_location.is_empty())
                {
                    parser::draw_error(file_loc, curr_line_content, "file's location", "was (null) or empty", c_line, j, lexer);
                    return false;
                }
                if (!io::file_exists(import_location))
                {
                    parser::draw_error(file_loc, curr_line_content, "file", import_location.wrap("'") + " not found", c_line, j, lexer);
                    return false;
                }
                // 100% correct syntax and file also exists
                // below block acts like a loop(recursion) for nested import functions
                parser temp_parser(import_location);
                if (!temp_parser.perform_parsing(lines_to_read))
                    return false;
                __parser__->operator+=(std::move(temp_parser)); // now whole file is parsed and "moved" to real map
                return true;
            }
            else
            {
                parser::draw_error(file_loc, curr_line_content, "expected", "\"", c_line, j, lexer);
                return false;
            }
        }
        return true;
    }

    bool parser::helper_parsing(parser *__parser__, openutils::sstring &lable, openutils::vector_t<openutils::vector_t<openutils::sstring>> &adding_vector, openutils::sstring &loc, const openutils::sstring &content, std::unordered_map<std::size_t, openutils::vector_t<openutils::vector_t<openutils::sstring>>> &parsed_data, std::size_t &curr_line, const unsigned int &lines_to_read)
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
                    j++; // skip [
                    adding_vector.erase();
                    lable.clear();

                    while (lexer[j].second() != openutils::lexer_token::NULL_END)
                    {
                        if (lexer[j].second() == openutils::lexer_token::WHITESPACE)
                        {
                            parser::draw_error(loc, split[i_split], "unexpected", "' '", curr_line, j, lexer);
                            return false;
                        }
                        else if (lexer[j].first() == "]") // ends
                            break;
                        else if (j >= lexer.length() - 1)
                        {
                            parser::draw_error(loc, split[i_split], "expected", "']'", curr_line, j, lexer);
                            return false;
                        }
                        lable += lexer[j].first();
                        j++;
                    }
                    // now at this point where were no errors
                    j++; // skip ]
                    if (j >= lexer.length() - 1)
                    {
                        parser::draw_error(loc, split[i_split], "expected", "':'", curr_line, j, lexer);
                        return false;
                    }
                    if (lexer[j].first() != ":")
                    {
                        parser::draw_error(loc, split[i_split], "expected", "':'", curr_line, j, lexer);
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
                        parser::draw_error(loc, split[i_split], "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                        return false;
                    }
                }
                else if (lexer[j].first() != "if" && lexer[j].first() != "#" && lexer[j].first() != "import")
                {
                    openutils::sstring var_name;
                    if (lable.is_empty() || lable.is_null())
                    {
                        parser::draw_error(loc, split[i_split], "cannot define a variable without a parent target", "", curr_line, j, lexer);
                        return false;
                    }
                    while (lexer[j].second() != openutils::lexer_token::NULL_END)
                    {
                        var_name += lexer[j++].first(); // storing variable name, just for target calling
                        if (lexer[j].second() == openutils::lexer_token::WHITESPACE || lexer[j].first() == "=" || lexer[j].first() == "(")
                            break;
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
                                    parser::draw_error(loc, split[i_split], "expected", "'", curr_line, j, lexer);
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
                                parser::draw_error(loc, split[i_split], "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
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
                                            parser::draw_error(loc, split[i_split], "expected", "'", curr_line, j, lexer);
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
                                        parser::draw_error(loc, split[i_split], "expected", "' or ']'", curr_line, j, lexer);
                                        return false;
                                    }
                                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                        j++; // ignore whitespaces
                                }
                                else
                                {
                                    parser::draw_error(loc, split[i_split], "expected", "'", curr_line, j, lexer);
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
                                parser::draw_error(loc, split[i_split], "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                return false;
                            }
                        }
                        else
                        {
                            parser::draw_error(loc, split[i_split], "expected", "' or '['", curr_line, j, lexer);
                            return false;
                        }
                        adding_vector.add(temp_vec);
                    }
                    else if (lexer[j].first() == "(")
                    {
                        j++; // skip (
                        if (!parsed_data.contains(var_name.hash()))
                        {
                            parser::draw_error(loc, split[i_split], "target", var_name.wrap("'") + " not found", curr_line, j, lexer);
                            return false;
                        }
                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                            j++; // ignore whitespaces

                        if (lexer[j].first() != ")")
                        {
                            parser::draw_error(loc, split[i_split], "expected", "')'", curr_line, j, lexer);
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
                                parser::draw_error(loc, split[i_split], "expected an positive integer", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                return false;
                            }
                            pos_sio = j;
                            select_index_outer = std::stoul(lexer[j].first().c_str()); // error will never happen because of above if statement
                            j++;                                                       // skip the index
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                j++; // ignore whitespaces
                            if (lexer[j].first() != "]")
                            {
                                parser::draw_error(loc, split[i_split], "expected", "']'", curr_line, j, lexer);
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
                                    parser::draw_error(loc, split[i_split], "expected an positive integer", lexer[j].first().wrap("'"), curr_line, j, lexer);
                                    return false;
                                }
                                pos_sii = j;
                                select_index_inner = std::stoul(lexer[j].first().c_str()); // error will never happen because of above if statement
                                j++;                                                       // skip the index
                                while (lexer[j].second() == openutils::lexer_token::WHITESPACE && lexer[j].second() != openutils::lexer_token::NULL_END)
                                    j++; // ignore whitespaces
                                if (lexer[j].first() != "]")
                                {
                                    parser::draw_error(loc, split[i_split], "expected", "']'", curr_line, j, lexer);
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
                            parser::draw_error(loc, split[i_split], "unexpected token", lexer[j].first().wrap("'"), curr_line, j, lexer);
                            return false;
                        }
                        // now syntax is 100% correct, now as the target which is going to be called is already parsed so we don't need to parse it again, just append its variable's values to current lable's values serial wise
                        // also, target is 100% present as checked above
                        // now, aftet the addition of getting target's particular value using index
                        // we have to manage both `select_index_outer` and `select_index_inner`
                        const openutils::vector_t<openutils::vector_t<openutils::sstring>> &temp_val = parsed_data.at(var_name.hash());
                        if (select_index_outer != static_cast<std::size_t>(-1))
                        {
                            if (select_index_outer >= temp_val.length())
                            {
                                parser::draw_error(loc, split[i_split], openutils::sstring("out-of-range max length was ") + openutils::sstring::to_sstring(temp_val.length()), openutils::sstring("but the given index was ") + openutils::sstring::to_sstring(select_index_outer), curr_line, pos_sio, lexer);
                                return false;
                            }
                            else
                            {
                                if (select_index_inner != static_cast<std::size_t>(-1))
                                {
                                    if (select_index_inner >= temp_val[select_index_outer].length())
                                    {
                                        parser::draw_error(loc, split[i_split], openutils::sstring("out-of-range max length was ") + openutils::sstring::to_sstring(temp_val[select_index_outer].length()), openutils::sstring("but the given index was ") + openutils::sstring::to_sstring(select_index_inner), curr_line, pos_sii, lexer);
                                        return false;
                                    }
                                    else
                                    {
                                        adding_vector.add({temp_val[select_index_outer][select_index_inner]});
                                    }
                                }
                                else
                                {
                                    adding_vector.add({temp_val[select_index_outer]});
                                }
                            }
                        }
                        else
                        {
                            adding_vector.add(temp_val);
                        }
                    }
                    else
                    {
                        parser::draw_error(loc, split[i_split], "expected", "'=' or '('", curr_line, j, lexer);
                        return false;
                    }
                    // here we assign the values to its respective target(lable)'s hash
                    parsed_data[lable.hash()].operator=(adding_vector);
                }
                else if (lexer[j].first() == "if")
                {
                    std::printf("GOT IF: '%s'\n", split[i_split].c_str());
                    break;
                }
                else if (lexer[j].first() == "#")
                {
                    // got a comment at starting of a line now, whole line is ignored
                    break; // skip that line
                }
                else if (lexer[j].first() == "import")
                {
                    if (!parser::import_helper(__parser__, lexer, loc, split[i_split], curr_line, lines_to_read))
                        return false;
                    // now import file's data has been parsed and stored, so we can skip the whole line at this point
                    break; // also, no errors were occurred
                }
                else
                {
                    parser::draw_error(loc, split[i_split], "expected", "'[', 'if', '<variable>' or 'import'", curr_line, j, lexer);
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
            temp_content.clear();
            temp_content.get() = chunk.first;         // transferring ownership of c-string
            temp_content.change_length(chunk.second); // changing length
            reader.make_nullptr();                    // we have transferred reader's ownership to temp_content

            if (!this->helper_parsing(this, this->M_lable, this->M_adding_vector, this->M_curr_location, temp_content, this->M_map, this->M_curr_line, max_lines))
                return false;

            chunk = reader.read_next();
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
                std::puts("[");
                for (std::size_t j = 0; j < val[i].length(); j++)
                {
                    std::printf("%s%s", val[i][j].c_str(), (j < val[i].length() - 1 ? ", " : "\n"));
                }
                std::puts("]");
            }
        }
    }

    parser &parser::operator+=(parser &&p)
    {
        if (this != &p && p.M_map.size() != 0)
        {
            for (const auto &[key, val] : p.M_map)
            {
                this->M_map[key].add(std::move(val));
            }
        }
        return *this;
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

                    inner_vec.add(str);
                }
                outer_vec.add(inner_vec);
            }
            p.M_map.insert({__key, outer_vec});
        }

        std::fclose(fptr);
        return true;
    }
}