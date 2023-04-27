#include "./parser.hh"

namespace runcpp
{
    void parser::draw_error(const openutils::sstring &loc, const openutils::sstring &line, const openutils::sstring &err_msg, const openutils::sstring &expected, std::size_t line_no, std::size_t curr_lexer, const openutils::heap_pair<openutils::sstring, openutils::lexer_token> *lexer_curr)
    {
        std::size_t col = 0;
        for (std::size_t r = 0; r < curr_lexer; r++)
            col += lexer_curr[r].first().length();
        std::fprintf(stderr, "err: at %s:%zu:%zu %s %s\n", loc.c_str(), line_no + 1, col + 1, err_msg.c_str(), expected.c_str());
        std::fprintf(stderr, "%s\n", line.c_str());
        std::fprintf(stderr, "%s^\n", openutils::sstring('~', col).c_str());
        std::exit(EXIT_FAILURE);
    }

    parser::parser(const openutils::sstring &content, const openutils::sstring &loc)
    {
        this->_M_content = content;
        this->_M_location = loc;
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
            openutils::vector_t<openutils::sstring> split;
            {
                if (this->_M_content.contains("\r\n"))
                    split = this->_M_content.split("\r\n");
                else
                    split = this->_M_content.split("\n");
            }
            openutils::sstring label; // global_s
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
                        if (lexer[j].first() != ":" && j < lexer.length())
                        {
                            this->draw_error(this->_M_location, split[i_split], "expected", "`:`", i_split, j, lexer.raw_data());
                        }
                        j++; // skip :
                        while (lexer[j].second() == openutils::lexer_token::WHITESPACE && j < lexer.length())
                            j++; // ignore whitspaces
                        if (j != lexer.length() - 1)
                        {
                            this->draw_error(this->_M_location, split[i_split], "unexpected token", openutils::sstring("`") + lexer[j].first() + openutils::sstring("`"), i_split, j, lexer.raw_data());
                        }
                    }
                    else if (lexer[j].first() != "#")
                    {
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
                            j++;
                            temp_child_vec.erase();
                            while (lexer[j].first() != "\"" && j < lexer.length())
                                temp_cmd += lexer[j++].first();
                            temp_child_vec.add(temp_cmd);
                            j++;
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
                        this->_M_map[label].operator=(adding_vector);
                    }
                }
            }
        }
        this->_M_content.clear(); // clears not-in-use memory
        return *this;
    }

    bool parser::contains_key(const openutils::sstring &__key) const
    {
        return this->_M_map.contains(__key);
    }

    void parser::print() const
    {
        for (const auto &[key, value] : this->_M_map)
        {
            std::cout << "`" << key << "`\n";
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
        if (this->_M_map.contains("global_s") && key != "global_s")
        {
            const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> &temp = this->_M_map.at("global_s");
            for (std::size_t i = 0; i < temp.length(); i++)
            {
                for (std::size_t j = 0; j < temp[i].second().length(); j++)
                {
                    ret_val.add(temp[i].second()[j]);
                }
            }
        }
        if (this->_M_map.contains(key))
        {
            const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> &temp = this->_M_map.at(key);
            for (std::size_t i = 0; i < temp.length(); i++)
            {
                for (std::size_t j = 0; j < temp[i].second().length(); j++)
                {
                    ret_val.add(temp[i].second()[j]);
                }
            }
        }
        if (this->_M_map.contains("global_e") && key != "global_e")
        {
            const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>> &temp = this->_M_map.at("global_e");
            for (std::size_t i = 0; i < temp.length(); i++)
            {
                for (std::size_t j = 0; j < temp[i].second().length(); j++)
                {
                    ret_val.add(temp[i].second()[j]);
                }
            }
        }
        return ret_val;
    }
}