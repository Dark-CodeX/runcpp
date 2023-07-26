/**
 * @file parser.hh
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#ifndef RUNCPP_PARSER_HH
#define RUNCPP_PARSER_HH

#include <openutils/sstring/sstring.hh>
#include <openutils/vector/vector.hh>
#include <unordered_map>
#include <cerrno>
#if defined _MSC_VER
#include <string> // only to support `stoul` function on Windows (MSVC)
#endif

#include "../io/io.hh"
#include "../os/os.hh"
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

        openutils::vector_t<openutils::sstring> M_target_vector; // this variable stores target's name only IF '--print-gui-client' flag is used

        openutils::sstring M_lable; // current target's name

        openutils::vector_t<openutils::vector_t<openutils::sstring>> M_adding_vector; // this vector stores the list of (value of a variable) in a target

        std::size_t M_curr_line; // current line

        bool store_target_names; // decideds whether 'M_target_vector' will store names or not

        bool M_was_if_true; // was condition of if true and does code under this block will execute

        bool M_was_else_true; // if true means if's condition was false, hence run only else block's code

        enum BLOCK_TYPE
        {
            NONE_BLOCK = 0,
            IF_BLOCK = 1,
            ELSE_BLOCK = 2
        };

        BLOCK_TYPE M_block; // indicates which block is code under (for config file)

    private: // private static functions
        static void draw_error(const openutils::sstring &loc, const openutils::sstring &err_msg, const openutils::sstring &expected, std::size_t line_no, std::size_t curr_lexer, const openutils::vector_t<std::pair<openutils::sstring, openutils::sstring_lexer_token>> &lexer);

        [[nodiscard]] static openutils::vector_t<openutils::sstring> convert_2d_vec_to_1d(const openutils::vector_t<openutils::vector_t<openutils::sstring>> &data);

        [[nodiscard]] static bool is_used_keyword(const std::size_t &__keyword_hash);

    private: // private static helper functions
        static void skip_comment(const openutils::vector_t<std::pair<openutils::sstring, openutils::sstring_lexer_token>> &lexer, std::size_t &j);

        static void skip_whitespaces_and_tabs(const openutils::vector_t<std::pair<openutils::sstring, openutils::sstring_lexer_token>> &lexer, std::size_t &j);

        [[nodiscard]] static bool validate_line_ending(const parser &ps, const openutils::vector_t<std::pair<openutils::sstring, openutils::sstring_lexer_token>> &lexer, std::size_t &j);

        [[nodiscard]] static bool import_helper(parser &ps, const openutils::vector_t<std::pair<openutils::sstring, openutils::sstring_lexer_token>> &lexer, const unsigned int &lines_to_read);

        [[nodiscard]] static bool helper_parsing(parser &ps, const openutils::sstring &content, const unsigned int &lines_to_read);

        [[nodiscard]] static bool evaluate_ifs(const openutils::vector_t<openutils::sstring> &lhs, const openutils::vector_t<openutils::sstring> &rhs);

        [[nodiscard]] static openutils::sstring validate_quotes(parser &ps, const openutils::sstring &quote_type, const openutils::vector_t<std::pair<openutils::sstring, openutils::sstring_lexer_token>> &lexer, std::size_t &j);

        [[nodiscard]] static openutils::vector_t<openutils::sstring> validate_array(parser &ps, const openutils::sstring &quote_type, const openutils::vector_t<std::pair<openutils::sstring, openutils::sstring_lexer_token>> &lexer, std::size_t &j);

    public: // deleted functions
        parser(const parser &) = delete;

        parser(parser &&) = delete;

        parser &operator=(const parser &) = delete;

        parser &operator=(parser &&) = delete;

    public: // real functions
        parser() = default;

        parser(const openutils::sstring &location, bool store_target = false);

        [[nodiscard]] bool perform_parsing(const unsigned int &max_lines);

        [[nodiscard]] openutils::vector_t<openutils::sstring> generate_command(const openutils::sstring &__key) const;

        [[nodiscard]] const openutils::vector_t<openutils::vector_t<openutils::sstring>> &generate_commands_all() const;

        [[nodiscard]] bool contains_key(const openutils::sstring &__key) const;

        /** @brief NOTE: this function does not clears unordered_map's data*/
        void clear_memory();

        void print() const;

        void print_for_gui_client() const;

        [[nodiscard]] bool merge(const openutils::sstring &out) const;

        [[nodiscard]] static bool serialize(const parser &p, const openutils::sstring &location);

        [[nodiscard]] static bool deserialize(parser &p, const openutils::sstring &location);
    };
}

#endif