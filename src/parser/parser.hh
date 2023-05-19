#ifndef PARSER_HH
#define PARSER_HH

#include <openutils/sstring/sstring.hh>
#include <openutils/vector/vector.hh>
#include <openutils/chunkio/chunkio_lines_reader.hh>
#include <unordered_map>

#include "../io/io.hh"
#include "../os/os.h"
#include "./used_keywords.h"
#include "../caller/caller.hh"

namespace runcpp
{
    class parser
    {
    private:                                // variables
        openutils::sstring M_curr_location; // current location

        // This variable stores the parsed data, here I am using `std::size_t` instead of `openutils::sstring` for KEY, to improve performance and reduce memory usage
        std::unordered_map<std::size_t, openutils::vector_t<openutils::vector_t<openutils::sstring>>> M_map;

        openutils::sstring M_lable;                                                   // target's name
        openutils::vector_t<openutils::vector_t<openutils::sstring>> M_adding_vector; // this vector stores the list of (value of a variable) in a target

        std::size_t M_curr_line; // current line

    private: // functions
        static void draw_error(const openutils::sstring &loc, const openutils::sstring &err_msg, const openutils::sstring &expected, std::size_t line_no, std::size_t curr_lexer, const openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::lexer_token>> &lexer);

        [[nodiscard]] static bool is_used_keyword(const std::size_t &__keyword_hash);

        [[nodiscard]] static bool import_helper(parser *__parser__, const openutils::vector_t<openutils::heap_pair<openutils::sstring, enum openutils::lexer_token>> &lexer, const openutils::sstring &file_loc, const std::size_t &c_line, const unsigned int &lines_to_read);

        [[nodiscard]] static bool helper_parsing(parser *__parser__, openutils::sstring &lable, openutils::vector_t<openutils::vector_t<openutils::sstring>> &adding_vector, openutils::sstring &loc, const openutils::sstring &content, std::unordered_map<std::size_t, openutils::vector_t<openutils::vector_t<openutils::sstring>>> &parsed_data, std::size_t &curr_line, const unsigned int &lines_to_read);

    public: // deleted functions
        parser() = default;
        parser(const parser &) = delete;
        parser(parser &&) = delete;
        parser &operator=(const parser &) = delete;
        parser &operator=(parser &&) = delete;

    public: // real functions
        parser(const openutils::sstring &location);
        [[nodiscard]] bool perform_parsing(const unsigned int &max_lines);
        [[nodiscard]] openutils::vector_t<openutils::sstring> generate_command(const openutils::sstring &__key) const;
        [[nodiscard]] bool contains_key(const openutils::sstring &__key) const;

        /** @brief NOTE: this function does not clears unordered_map's data*/
        void clear_memory();

        void print() const;

        parser &operator+=(parser &&p); // for adding M_map of two parsed objects

        [[nodiscard]] static bool serialize(const parser &p, const openutils::sstring &location);
        [[nodiscard]] static bool deserialize(parser &p, const openutils::sstring &location);
    };
}

#endif