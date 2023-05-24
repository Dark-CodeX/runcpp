#include "./io.hh"

namespace runcpp_format
{
    bool io::file_exists(const openutils::sstring &loc)
    {
        if (loc.is_null())
            return false;
        struct stat buffer = {};
        return (stat(loc.c_str(), &buffer) == 0);
    }
}