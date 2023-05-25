/**
 * @file os.c
 * @license This file is licensed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007. You may obtain a copy of this license at https://www.gnu.org/licenses/gpl-3.0.en.html.
 * @author Tushar Chaurasia (Dark-CodeX)
 */

#include "./os.h"

const char *get_os(void)
{
#if defined __linux__ || defined linux || defined __linux
    return "linux";
#elif _WIN32 || defined _WIN64 || defined __CYGWIN__
    return "windows";
#elif defined __unix__ || defined __unix || defined unix
    return "unix";
#elif defined __APPLE__ || defined __MACH__
    return "mac";
#endif
}

unsigned int get_app_arch(void)
{
#if defined __x86_64__ || defined __ppc64__
    return 64;
#else
    return 32;
#endif
}