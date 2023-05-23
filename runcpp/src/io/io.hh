#ifndef IO_HH
#define IO_HH

#include <openutils/sstring/sstring.hh>
#include <sys/stat.h>

namespace runcpp
{
    class io
    {
    public:
        /**
         * @return true if file exists;
         * @return false if file DOES NOT exists
         */
        [[nodiscard]] static bool file_exists(const openutils::sstring &loc);
    };
}
#endif
