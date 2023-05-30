/**
 * @file miscellaneous.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./parser.hh"

namespace runcpp
{
    openutils::vector_t<openutils::sstring> parser::convert_2d_vec_to_1d(const openutils::vector_t<openutils::vector_t<openutils::sstring>> &data)
    {
        openutils::vector_t<openutils::sstring> _ret_val_(data.length()); // TDOD: needs optimization
        for (std::size_t i = 0; i < data.length(); i++)
        {
            for (std::size_t j = 0; j < data[i].length(); j++)
            {
                _ret_val_.add(data[i][j]);
            }
        }
        return _ret_val_;
    }

    bool parser::evaluate_ifs(const openutils::vector_t<openutils::sstring> &lhs, const openutils::vector_t<openutils::sstring> &rhs)
    {
        if (lhs.length() == 1 && rhs.length() == 1)
        {
            // some constant comparssion
            const openutils::sstring &ele_lhs_0 = lhs[0];
            const openutils::sstring &ele_rhs_0 = rhs[0];
            if (ele_lhs_0 == "os")
            {
                return ele_rhs_0 == get_os();
            }
            else if (ele_lhs_0 == "os_arch")
            {
                return ele_rhs_0 == openutils::sstring::to_sstring(get_app_arch());
            }
        }
        return lhs == rhs;
    }

    bool parser::is_used_keyword(const std::size_t &__keyword_hash)
    {
        const std::size_t n = sizeof(used_keywords) / sizeof(used_keywords[0]);
        for (std::size_t i = 0; i < n; i++)
            if (__keyword_hash == used_keywords[i])
                return true;
        return false;
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
                std::printf("[");
                for (std::size_t j = 0; j < val[i].length(); j++)
                {
                    std::printf("%s%s", val[i][j].c_str(), (j < val[i].length() - 1 ? ", " : ""));
                }
                std::puts("]");
            }
            std::printf("\n"); // prints new line
        }
    }
}