#ifndef __GLOBALS_H__
#define __GLOBALS_H__


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
# include <windows.h>
# define WINDOWS_OS
#else
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>
#endif

#define GUI_ERRORS

#endif