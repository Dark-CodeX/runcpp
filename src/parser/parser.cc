#include "./parser.hh"

namespace runcpp
{
    void parser::draw_error(const openutils::sstring &loc, const openutils::sstring &line, const openutils::sstring &err_msg, const openutils::sstring &expected, std::size_t line_no, std::size_t curr_lexer, const openutils::heap_pair<openutils::sstring, openutils::lexer_token> *lexer_curr)
    {
        std::size_t col = 0;
        for (std::size_t r = 0; r < curr_lexer; r++)
            col += lexer_curr[r].first().length();
        std::fprintf(stderr, "err: at %s:%zu:%zu: %s %s\n", loc.c_str(), line_no + 1, col + 1, err_msg.c_str(), expected.c_str());
        std::fprintf(stderr, "%s\n", line.c_str());
        std::fprintf(stderr, "%s^\n", (col == 0 ? "" : openutils::sstring('~', col).c_str()));
        std::exit(EXIT_FAILURE);
    }

    parser::parser(const openutils::sstring &content, const openutils::sstring &loc)
    {
        this->_M_content = content;
        this->_M_location = loc;
        this->_M_curr_location = loc;
    }

    void parser::import_helper()
    {
        if (!this->_M_content.contains("import"))
        {
            this->_M_locations_imported.clear();
            return;
        }
        openutils::vector_t<openutils::sstring> split;
        {
            if (this->_M_content.contains("\r\n"))
                split = this->_M_content.split("\r\n");
            else
                split = this->_M_content.split("\n");
        }

        for (std::size_t i_split = 0; i_split < split.length(); i_split++)
        {
            openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> lexer = split[i_split].lexer();
            std::size_t j = 0;
            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                j++; // ignore whitspaces
            while (lexer[j].second() != openutils::lexer_token::NULL_END)
            {
                if (lexer[j].first() == "import")
                {
                    j++; // skip import keyword
                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                        j++; // ignore whitspaces
                    if (lexer[j].first() == "\"")
                    {
                        j++;                                             // skip "
                        this->_M_prev_location = this->_M_curr_location; // copy prev location
                        this->_M_curr_location.clear();                  // current location
                        while (lexer[j].first() != "\"" && j < lexer.length())
                            this->_M_curr_location += lexer[j++].first();
                        j++; // skip "
                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                            j++; // ignore whitspaces
                        if (j != lexer.length() - 1)
                        {
                            this->draw_error(this->_M_prev_location, split[i_split], "unexpected token", openutils::sstring("`") + lexer[j].first() + openutils::sstring("`"), i_split, j, lexer.raw_data());
                        }
                        // stop infinite recursion if same file is imported
                        if (this->_M_locations_imported.contains(this->_M_curr_location))
                        {
                            this->draw_error(this->_M_prev_location, split[i_split], "either importing the same file itself or importing another file which imports its parent file will cause infinite recursion", "", i_split, j, lexer.raw_data());
                        }
                        // inserting that file's location
                        if (!io::file_exists(this->_M_curr_location))
                        {
                            this->draw_error(this->_M_prev_location, split[i_split], "file not found", openutils::sstring("`") + this->_M_curr_location + openutils::sstring("`"), i_split, j, lexer.raw_data());
                        }
                        openutils::sstring _temp_file_;
                        _temp_file_.open(this->_M_curr_location);

                        this->_M_content.replace(split[i_split], _temp_file_);
                        this->_M_locations_imported.insert(this->_M_curr_location);
                        // now doing recursion
                        this->import_helper();
                    }
                    else
                    {
                        this->draw_error(this->_M_curr_location, split[i_split], "expected", "\"", i_split, j, lexer.raw_data());
                    }
                }
                else
                {
                    break; // not import lines
                }
            }
        }
    }

    parser &parser::perform_parsing()
    {
        if (this->_M_content.is_null())
        {
            std::fprintf(stderr, "err: no input file\n");
            std::exit(EXIT_FAILURE);
        }
        else
        {
            this->import_helper(); // resolves imports if any import file was given
            openutils::vector_t<openutils::sstring> split;
            {
                if (this->_M_content.contains("\r\n"))
                    split = this->_M_content.split("\r\n");
                else
                    split = this->_M_content.split("\n");
            }
            openutils::sstring label; // global_s
            bool include_global = false;
            openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> adding_vector;

            for (std::size_t i_split = 0; i_split < split.length(); i_split++)
            {
                openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> lexer = split[i_split].lexer();
                std::size_t j = 0;
                while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                    j++; // ignore whitspaces
                while (lexer[j].second() != openutils::lexer_token::NULL_END)
                {
                    if (lexer[j].first() == "[")
                    {
                        j++; // now skip [
                        include_global = false;
                        label.clear();
                        adding_vector.erase();
                        while (lexer[j].first() != "]" && j < lexer.length())
                        {
                            if (lexer[j].second() == openutils::lexer_token::WHITESPACE)
                            {
                                this->draw_error(this->_M_location, split[i_split], "unexpected", "` `", i_split, j, lexer.raw_data());
                            }
                            label += lexer[j].first();
                            j++;
                        }
                        j++; // skip ]
                        if (lexer[j].first() != ":" && lexer[j].first() != "X" && j < lexer.length())
                        {
                            this->draw_error(this->_M_location, split[i_split], "expected", "`:` or `X`", i_split, j, lexer.raw_data());
                        }
                        if (lexer[j].first() == "X")
                        {
                            include_global = true;
                            if (label == "global_s" || label == "global_e")
                            {
                                this->draw_error(this->_M_location, split[i_split], "unexpected", "`X`, global target cannot be assigned with `X`", i_split, j, lexer.raw_data());
                            }
                            j++; // skip X
                            if (lexer[j].first() != ":" && j < lexer.length())
                            {
                                this->draw_error(this->_M_location, split[i_split], "expected", "`:`", i_split, j, lexer.raw_data());
                            }
                            j++; // skip :
                        }
                        else
                            j++; // skip : with NOT X
                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                            j++; // ignore whitspaces
                        if (j != lexer.length() - 1)
                        {
                            this->draw_error(this->_M_location, split[i_split], "unexpected token", openutils::sstring("`") + lexer[j].first() + openutils::sstring("`"), i_split, j, lexer.raw_data());
                        }
                    }
                    else if (lexer[j].first() != "if")
                    {
                        if (label.length() == 0 || label.is_null())
                        {
                            this->draw_error(this->_M_location, split[i_split], "cannot define a variable without a parent target", "", i_split, j, lexer.raw_data());
                        }
                        openutils::sstring temp_lable;                          // compiler
                        openutils::sstring temp_cmd;                            // "g++"
                        openutils::vector_t<openutils::sstring> temp_child_vec; // [-g", "-O3"]
                        while (j < lexer.length())
                        {
                            temp_lable += lexer[j++].first();
                            if (lexer[j].second() == openutils::lexer_token::WHITESPACE || lexer[j].first() == "=")
                                break;
                        }
                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                            j++; // ignore whitspaces
                        if (lexer[j].first() != "=")
                        {
                            this->draw_error(this->_M_location, split[i_split], "expected", "`=`", i_split, j, lexer.raw_data());
                        }
                        j++; // skip =
                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                            j++; // ignore whitspaces

                        // now check if there is " or [
                        if (lexer[j].first() == "\"")
                        {
                            j++; // skip "
                            temp_child_vec.erase();
                            while (lexer[j].first() != "\"" && j < lexer.length())
                                temp_cmd += lexer[j++].first();
                            temp_child_vec.add(temp_cmd);
                            j++; // skip "
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                                j++; // ignore whitspaces
                            if (j != lexer.length() - 1)
                            {
                                this->draw_error(this->_M_location, split[i_split], "unexpected token", openutils::sstring("`") + lexer[j].first() + openutils::sstring("`"), i_split, j, lexer.raw_data());
                            }
                            adding_vector.add({temp_lable, temp_child_vec});
                        }
                        else if (lexer[j].first() == "[")
                        {
                            j++;
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                                j++; // ignore whitspaces
                            temp_child_vec.erase();
                            while (lexer[j].first() != "]" && j < lexer.length())
                            {
                                openutils::sstring temp_child_cmd;
                                if (lexer[j].first() == "\"")
                                {
                                    j++; // skip "
                                    while (lexer[j].first() != "\"" && j < lexer.length())
                                        temp_child_cmd += lexer[j++].first();
                                    temp_child_vec.add(temp_child_cmd);
                                    j++; // skip "
                                    if (lexer[j].first() == " ")
                                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                                            j++; // ignore whitspaces
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
                                        this->draw_error(this->_M_location, split[i_split], "expected", "`\"` or `]`", i_split, j, lexer.raw_data());
                                    }
                                    while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                                        j++; // ignore whitspaces
                                }
                                else
                                {
                                    this->draw_error(this->_M_location, split[i_split], "expected", "`\"`", i_split, j, lexer.raw_data());
                                }
                            }
                            while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                                j++; // ignore whitspaces
                            if (j != lexer.length() - 1)
                            {
                                this->draw_error(this->_M_location, split[i_split], "unexpected token", openutils::sstring("`") + lexer[j].first() + openutils::sstring("`"), i_split, j, lexer.raw_data());
                            }
                            adding_vector.add({temp_lable, temp_child_vec});
                        }
                        else
                        {
                            this->draw_error(this->_M_location, split[i_split], "expected", "`\"` or `[`", i_split, j, lexer.raw_data());
                        }
                        this->_M_map[{label, include_global}].operator=(adding_vector);
                    }
                    else if (lexer[j].first() == "if")
                    {
                        std::cout << "IF: " << split[i_split] << "\n";
                        break;
                    }
                    else
                    {
                        this->draw_error(this->_M_location, split[i_split], "expected", "`[`, `if` or `<variable>`", i_split, j, lexer.raw_data());
                    }
                }
            }
        }
        this->_M_content.clear(); // clears not-in-use memory
        return *this;
    }

    bool parser::contains_key(const openutils::sstring &__key) const
    {
        if (!this->_M_map.contains({__key, false}))
            if (!this->_M_map.contains({__key, true}))
                return false;
        return true;
    }

    void parser::print() const
    {
        for (const auto &[key, value] : this->_M_map)
        {
            std::cout << "`" << key.first() << "`\n";
            for (std::size_t i = 0; i < value.length(); i++)
            {
                std::cout << "{\n\t`" << value[i].first() << "`\t\n\t{\n";
                for (std::size_t j = 0; j < value[i].second().length(); j++)
                {
                    std::cout << "\t`" << value[i].second()[j] << "`,\n";
                }
                std::cout << "\t}\n}\n\n";
            }
        }
    }

    openutils::vector_t<openutils::sstring> parser::generate_command(const openutils::sstring &key) const
    {
        openutils::vector_t<openutils::sstring> ret_val;
        if (key == "global_s")
        {
            if (!this->_M_map.contains({key, false}))
                return ret_val;
            const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> &temp = this->_M_map.at({key, false});
            for (std::size_t i = 0; i < temp.length(); i++)
            {
                for (std::size_t j = 0; j < temp[i].second().length(); j++)
                {
                    ret_val.add(temp[i].second()[j]);
                }
            }
            return ret_val;
        }
        else if (key == "global_e")
        {
            if (!this->_M_map.contains({key, false}))
                return ret_val;
            const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> &temp = this->_M_map.at({key, false});
            for (std::size_t i = 0; i < temp.length(); i++)
            {
                for (std::size_t j = 0; j < temp[i].second().length(); j++)
                {
                    ret_val.add(temp[i].second()[j]);
                }
            }
            return ret_val;
        }
        else
        {
            if (!this->contains_key(key))
                return ret_val;
            if (this->_M_map.contains({key, true}))
            {
                const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> &temp = this->_M_map.at({key, true});
                for (std::size_t i = 0; i < temp.length(); i++)
                {
                    for (std::size_t j = 0; j < temp[i].second().length(); j++)
                    {
                        ret_val.add(temp[i].second()[j]);
                    }
                }
            }
            else
            {
                if (this->_M_map.contains({"global_s", false}))
                {
                    const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> &temp = this->_M_map.at({"global_s", false});
                    for (std::size_t i = 0; i < temp.length(); i++)
                    {
                        for (std::size_t j = 0; j < temp[i].second().length(); j++)
                        {
                            ret_val.add(temp[i].second()[j]);
                        }
                    }
                }
                {
                    const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> &temp = this->_M_map.at({key, false});
                    for (std::size_t i = 0; i < temp.length(); i++)
                    {
                        for (std::size_t j = 0; j < temp[i].second().length(); j++)
                        {
                            ret_val.add(temp[i].second()[j]);
                        }
                    }
                }
                if (this->_M_map.contains({"global_e", false}))
                {
                    const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> &temp = this->_M_map.at({"global_e", false});
                    for (std::size_t i = 0; i < temp.length(); i++)
                    {
                        for (std::size_t j = 0; j < temp[i].second().length(); j++)
                        {
                            ret_val.add(temp[i].second()[j]);
                        }
                    }
                }
            }
            return ret_val;
        }
    }

    parser::~parser()
    {
        this->_M_content.clear();
        this->_M_location.clear();
        this->_M_curr_location.clear();
        this->_M_locations_imported.clear();
        this->_M_prev_location.clear();
        this->_M_map.clear();
    }
}