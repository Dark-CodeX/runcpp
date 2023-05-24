#ifndef RUNCPP_FORMAT_IO_HH
#define RUNCPP_FORMAT_IO_HH

#include <openutils/sstring/sstring.hh>
#include <sys/stat.h>

namespace runcpp_format
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
