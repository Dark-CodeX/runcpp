#ifndef COMMAND_LINE_HH
#define COMMAND_LINE_HH

#include <openutils/sstring/sstring.hh>
#include <openutils/vector/vector.hh>

namespace runcpp
{
    class command_line
    {
        public:
            static openutils::vector_t<openutils::sstring> parse_command_line(int argc, const char **argv);
    };
}

#endif