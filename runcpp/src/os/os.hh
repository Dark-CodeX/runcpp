/**
 * @file os.hh
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#ifndef RUNCPP_OS_HH
#define RUNCPP_OS_HH

namespace runcpp
{
    class os
    {
    public:
        [[nodiscard]] static const char *get_os(void);
        [[nodiscard]] static unsigned int get_app_arch(void);
    };
}

#endif