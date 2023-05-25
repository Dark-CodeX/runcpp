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
}