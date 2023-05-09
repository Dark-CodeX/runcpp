#ifndef IO_HH
#define IO_HH

#include <openutils/sstring/sstring.hh>
#include <sys/stat.h>

namespace runcpp
{
    class io
    {
    public:
        static openutils::sstring read_file(const openutils::sstring &loc);
        static bool save_file(const openutils::sstring &loc, const openutils::sstring &content);
        static bool file_exists(const openutils::sstring &loc);
    };
}
#endif
