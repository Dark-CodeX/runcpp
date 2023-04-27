#include "./io.hh"

namespace runcpp
{
    openutils::sstring io::read_file(const openutils::sstring &loc)
    {
        openutils::sstring x;
        x.open(loc);
        return std::move(x);
    }

    bool io::save_file(const openutils::sstring &loc, const openutils::sstring &content)
    {
        return content.save(loc);
    }

    bool io::file_exists(const openutils::sstring &loc)
    {
        if (loc.is_null())
            return false;
        struct stat buffer = {};
        return (stat(loc.c_str(), &buffer) == 0);
    }
}