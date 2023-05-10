#ifndef IO_HH
#define IO_HH

#include <openutils/sstring/sstring.hh>
#include <sys/stat.h>

namespace runcpp
{
    class io
    {
    public:
        static bool file_exists(const openutils::sstring &loc);
    };
}
#endif
