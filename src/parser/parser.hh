#ifndef PARSER_HH
#define PARSER_HH

#include <openutils/sstring/sstring.hh>
#include <openutils/vector/vector.hh>
#include <unordered_map>
#include <unordered_set>
#include "../io/io.hh"
#include <iostream>

namespace runcpp
{
    class parser
    {
      private:
        openutils::sstring _M_content, _M_location, _M_curr_location, _M_prev_location;
        std::unordered_set<openutils::sstring> _M_locations_imported;
        std::unordered_map<openutils::heap_pair<openutils::sstring, bool>, openutils::vector_t<openutils::heap_pair<openutils::sstring, openutils::vector_t<openutils::sstring>>>> _M_map;
        static void draw_error(const openutils::sstring &loc, const openutils::sstring &line, const openutils::sstring &err_msg, const openutils::sstring &expected, std::size_t line_no, std::size_t curr_lexer, const openutils::heap_pair<openutils::sstring, openutils::lexer_token> *lexer_curr);
        void import_helper();

      public:
        parser(const openutils::sstring &content, const openutils::sstring &loc);
        parser &perform_parsing();
        bool contains_key(const openutils::sstring &__key) const;
        void print() const;
        void clear_memory();
        openutils::vector_t<openutils::sstring> generate_command(const openutils::sstring &key) const;
    };
}

#endif