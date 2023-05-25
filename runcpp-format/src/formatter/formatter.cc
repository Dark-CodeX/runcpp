/**
 * @file formatter.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./formatter.hh"

namespace runcpp_format
{
    bool formatter::formatting_helper(const openutils::sstring &content, openutils::sstring &appending_content)
    {
        openutils::vector_t<openutils::sstring> split = content.split("\n");
        for (std::size_t i_split = 0; i_split < split.length(); i_split++)
        {
            openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> lexer = split[i_split].lexer();
            std::size_t j = 0;
        }
        return true;
    }

    formatter::formatter(const openutils::sstring &loc)
    {
        this->M_location = loc;
    }

    bool formatter::do_formatting(const unsigned int &max_lines)
    {
        if (this->M_location.is_null() || this->M_location.is_empty() || io::file_exists(this->M_location) == false)
        {
            std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' was not found.\n", (this->M_location.is_empty() ? (this->M_location.is_null() ? "(null)" : this->M_location.c_str()) : this->M_location.c_str()));
            return false;
        }
        openutils::chunkio_lines_reader<char> reader(this->M_location.c_str(), max_lines);

        std::pair<char *&, std::size_t> chunk = reader.read_next();

        openutils::sstring temp_content;
        while (chunk.first)
        {
            if (reader.is_file_binary())
            {
                std::fprintf(stderr, "\033[1;91merr:\033[0m given file '%s' was binary, which cannot be formatted.\n", this->M_location.c_str());
                return false;
            }
            temp_content.clear();
            temp_content.get() = chunk.first;         // transferring ownership of c-string
            temp_content.change_length(chunk.second); // changing length
            reader.make_nullptr();                    // we have transferred reader's ownership to temp_content

            if (!this->formatting_helper(temp_content, this->M_formatted_content))
                return false;

            chunk = reader.read_next();
        }
        return true;
    }

    bool formatter::save_or_overwrite()
    {
        bool ret_val = this->M_formatted_content.save(this->M_location);
        this->M_formatted_content.clear(); // file is saved no need to keep data in memory
        this->M_location.clear();          // file is saved no need to keep data in memory
        return ret_val;
    }
}