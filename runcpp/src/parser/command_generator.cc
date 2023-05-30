/**
 * @file command_generator.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./parser.hh"

namespace runcpp
{
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

    const openutils::vector_t<openutils::vector_t<openutils::sstring>> &parser::generate_commands_all() const
    {
        // its the caller's responsibility to check if `all` target exists or NOT
        return this->M_map.at(openutils::sstring("all").hash());
    }
}