/**
 * @file serialize_deserialize_merge.cc
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./parser.hh"

namespace runcpp
{
    bool parser::merge(const openutils::sstring &out) const
    {
        std::FILE *fptr = std::fopen(out.c_str(), "wb");
        if (!fptr)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m could not open file '%s' for writing: %s\n", out.c_str(), std::strerror(errno));
            else
                std::fprintf(stderr, "err: could not open file '%s' for writing: %s\n", out.c_str(), std::strerror(errno));
            return false;
        }
        for (const auto &[key, val] : this->M_map)
        {
            openutils::sstring cont = "[";
            cont += openutils::sstring::to_sstring(key);
            cont += "]:\n";
            for (std::size_t i = 0; i < val.length(); i++)
            {
                cont += openutils::sstring::to_sstring(i) + " = [";
                for (std::size_t j = 0; j < val[i].length(); j++)
                {
                    cont += val[i][j].wrap("'") + (j < val[i].length() - 1 ? ", " : "");
                }
                cont += "]\n";
            }
            if (std::fwrite(cont.c_str(), sizeof(char), cont.length(), fptr) != cont.length())
            {
                if (colorize::should_colorize(colorize::STDPTR::ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m could not save file at '%s': %s\n", out.c_str(), std::strerror(errno));
                else
                    std::fprintf(stderr, "err: could not save file at '%s': %s\n", out.c_str(), std::strerror(errno));
                std::fclose(fptr);
                return false;
            }
        }
        std::fclose(fptr);
        return true;
    }

    bool parser::serialize(const parser &p, const openutils::sstring &location)
    {
        std::FILE *fptr = std::fopen(location.c_str(), "wb");
        if (!fptr)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m could not open file '%s' for writing: %s\n", location.c_str(), std::strerror(errno));
            else
                std::fprintf(stderr, "err: could not open file '%s' for writing: %s\n", location.c_str(), std::strerror(errno));
            return false;
        }

        bool is_app_arch_64 = (os::get_app_arch() == 64 ? true : false);
        if (std::fwrite(&is_app_arch_64, sizeof(is_app_arch_64), 1, fptr) != 1)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
            else
                std::fprintf(stderr, "err: file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
            return false;
        }

        std::size_t desc_len = 46;
        if (std::fwrite(&desc_len, sizeof(desc_len), 1, fptr) != 1)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
            else
                std::fprintf(stderr, "err: file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
            return false;
        }
        const char *desc = "generated using runcpp, don't change manually";
        if (std::fwrite(desc, sizeof(char), desc_len, fptr) != desc_len)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
            else
                std::fprintf(stderr, "err: file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
            return false;
        }

        std::size_t umap_len = p.M_map.size();
        if (std::fwrite(&umap_len, sizeof(umap_len), 1, fptr) != 1)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
            else
                std::fprintf(stderr, "err: file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
            return false;
        }

        for (const auto &[key, val] : p.M_map)
        {
            std::size_t __key = key;
            if (std::fwrite(&__key, sizeof(__key), 1, fptr) != 1)
            {
                if (colorize::should_colorize(colorize::STDPTR::ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                else
                    std::fprintf(stderr, "err: file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                return false;
            }

            std::size_t outer_vec_len = val.length();
            if (std::fwrite(&outer_vec_len, sizeof(outer_vec_len), 1, fptr) != 1)
            {
                if (colorize::should_colorize(colorize::STDPTR::ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                else
                    std::fprintf(stderr, "err: file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                return false;
            }

            for (std::size_t i = 0; i < val.length(); i++)
            {
                std::size_t inner_vec_len = val[i].length();
                if (std::fwrite(&inner_vec_len, sizeof(inner_vec_len), 1, fptr) != 1)
                {
                    if (colorize::should_colorize(colorize::STDPTR::ERR))
                        std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                    else
                        std::fprintf(stderr, "err: file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                    return false;
                }

                for (std::size_t j = 0; j < val[i].length(); j++)
                {
                    std::size_t str_len = val[i][j].length();
                    if (std::fwrite(&str_len, sizeof(str_len), 1, fptr) != 1)
                    {
                        if (colorize::should_colorize(colorize::STDPTR::ERR))
                            std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                        else
                            std::fprintf(stderr, "err: file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                        return false;
                    }
                    if (std::fwrite(val[i][j].c_str(), sizeof(char), str_len, fptr) != str_len)
                    {
                        if (colorize::should_colorize(colorize::STDPTR::ERR))
                            std::fprintf(stderr, "\033[1;91merr:\033[0m file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                        else
                            std::fprintf(stderr, "err: file '%s' could not be saved: %s\n", location.c_str(), std::strerror(errno));
                        return false;
                    }
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
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m could not open file '%s' for reading: %s\n", location.c_str(), std::strerror(errno));
            else
                std::fprintf(stderr, "err: could not open file '%s' for reading: %s\n", location.c_str(), std::strerror(errno));
            return false;
        }

        bool is_binary_64;
        if (std::fread(&is_binary_64, sizeof(is_binary_64), 1, fptr) != 1)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
            else
                std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }

        if (is_binary_64 == true && os::get_app_arch() == 32) // means app is 32 bit, but bin is 64 bit
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is 64 bit whereas, the app/os is 32 bit.\n", location.c_str());
            else
                std::fprintf(stderr, "err: given binary file ('%s') is 64 bit whereas, the app/os is 32 bit.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }
        if (is_binary_64 == false && os::get_app_arch() == 64) // means app is 64 bit, but bin is 32 bit
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') is 32 bit whereas, the app/os is 64 bit.\n", location.c_str());
            else
                std::fprintf(stderr, "err: given binary file ('%s') is 32 bit whereas, the app/os is 64 bit.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }

        std::size_t desc_len;
        if (std::fread(&desc_len, sizeof(desc_len), 1, fptr) != 1)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
            else
                std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }
        if (desc_len != 46)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
            else
                std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }
        openutils::sstring desc('\0', desc_len);
        std::fread(desc.get(), sizeof(char), desc_len, fptr);
        if (desc != "generated using runcpp, don't change manually")
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
            else
                std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
            std::fclose(fptr);
            return false;
        }

        std::size_t umap_len;
        if (std::fread(&umap_len, sizeof(umap_len), 1, fptr) != 1)
        {
            if (colorize::should_colorize(colorize::STDPTR::ERR))
                std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
            else
                std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
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
                if (colorize::should_colorize(colorize::STDPTR::ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
                else
                    std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
                std::fclose(fptr);
                return false;
            }
            std::size_t outer_vec_len;
            if (std::fread(&outer_vec_len, sizeof(outer_vec_len), 1, fptr) != 1)
            {
                if (colorize::should_colorize(colorize::STDPTR::ERR))
                    std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
                else
                    std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
                std::fclose(fptr);
                return false;
            }

            openutils::vector_t<openutils::vector_t<openutils::sstring>> outer_vec(outer_vec_len);

            for (std::size_t j = 0; j < outer_vec_len; j++)
            {
                std::size_t inner_vec_len;
                if (std::fread(&inner_vec_len, sizeof(inner_vec_len), 1, fptr) != 1)
                {
                    if (colorize::should_colorize(colorize::STDPTR::ERR))
                        std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
                    else
                        std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
                    std::fclose(fptr);
                    return false;
                }

                openutils::vector_t<openutils::sstring> inner_vec(inner_vec_len);

                for (std::size_t k = 0; k < inner_vec_len; k++)
                {
                    std::size_t str_len;
                    if (std::fread(&str_len, sizeof(str_len), 1, fptr) != 1)
                    {
                        if (colorize::should_colorize(colorize::STDPTR::ERR))
                            std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
                        else
                            std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
                        std::fclose(fptr);
                        return false;
                    }

                    openutils::sstring str('\0', str_len);
                    if (std::fread(str.get(), sizeof(char), str_len, fptr) != str_len)
                    {
                        if (colorize::should_colorize(colorize::STDPTR::ERR))
                            std::fprintf(stderr, "\033[1;91merr:\033[0m given binary file ('%s') was modified file by external agent.\n", location.c_str());
                        else
                            std::fprintf(stderr, "err: given binary file ('%s') was modified file by external agent.\n", location.c_str());
                        std::fclose(fptr);
                        return false;
                    }

                    inner_vec.add(std::move(str));
                }
                outer_vec.add(std::move(inner_vec));
            }
            p.M_map[__key].operator=(std::move(outer_vec));
        }

        std::fclose(fptr);
        return true;
    }
}