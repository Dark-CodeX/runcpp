/**
 * @file io.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./io.hh"

namespace runcpp
{
    bool io::file_exists(const openutils::sstring &loc)
    {
        if (loc.is_null())
            return false;
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
        struct _stat buffer = {};
        return (_stat(loc.c_str(), &buffer) == 0);

#else
        struct stat buffer = {};
        return (stat(loc.c_str(), &buffer) == 0);
#endif
    }

    bool io::directory_exists(const openutils::sstring &loc)
    {
        if (loc.is_null())
            return false;
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
        DWORD fileAttributes = GetFileAttributesA(loc.c_str());
        if (fileAttributes == INVALID_FILE_ATTRIBUTES)
            return false;
        else if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            return true;
        else
            return false;
#else
        struct stat buffer = {};
        if (stat(loc.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode))
            return true;
#endif
        return false;
    }

    io_reader::io_reader(const std::size_t &lines_to_read)
    {
        this->M_fptr = nullptr;
        this->M_lines_to_read = lines_to_read;
    }

    bool io_reader::open_file(const openutils::sstring &loc)
    {
        this->M_fptr = std::fopen(loc.c_str(), "rb");
        if (!this->M_fptr)
            return false;
        std::fseek(this->M_fptr, 0, SEEK_SET);
        return true;
    }

    openutils::sstring &io_reader::read_next()
    {
        if (this->M_fptr)
        {
            if (std::feof(this->M_fptr))
            {
                this->M_content.clear();
                return this->M_content;
            }
            this->M_content.clear();
            std::size_t curr_line = 0, len = 0, cap = 128;
            this->M_content.resize(cap);

            while (!std::feof(this->M_fptr) && curr_line != this->M_lines_to_read)
            {
                std::fread(this->M_content.get() + len, sizeof(char), 1, this->M_fptr);
                if (this->M_content.c_str()[len] == 10)
                    curr_line++;
                this->M_content.change_length(++len);
                if (len == cap)
                {
                    cap *= 2;
                    this->M_content.resize(cap);
                }
            }
        }
        return this->M_content;
    }

    io_reader::~io_reader()
    {
        if (this->M_fptr)
            std::fclose(this->M_fptr);
        this->M_content.clear();
    }
}