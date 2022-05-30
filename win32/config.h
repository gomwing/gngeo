#ifndef GNGEO_CONFIG_H
#define GNGEO_CONFIG_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

/* Define to 1 if you have the `atexit' function. */
#define HAVE_ATEXIT 1
/* Define to 1 if you have the `getopt_long' function. */
#define HAVE_GETOPT_LONG 1
/* Version number of package */
#define VERSION "0.7a"
#define DATA_DIRECTORY "."


/* TO CHECK */
#undef HAVE_SCANDIR
//#define CHECK_ALLOC(x)
#define USE_STARSCREAM
#define USE_RAZE
#define HAVE_LIBZ
#ifdef _MSC_VER 
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strdup _strdup 
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define mkdir _mkdir
#endif
#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif

#define realpath(x,y) (x)
#define lstat stat

// #ifndef S_ISLNK
// #define S_ISREG(mode)  (0)
// #endif
#define GN_MAX_KEY	16

static inline void _dbg(char* format, ...)
{
    //char	buff[128];
    //char* stringptr = buff;
    //va_list		   argp;
    //va_start(argp, format);
    //vsprintf(buff, format, argp);
    //va_end(argp);
    ////gm_sprintf(buff,format,arg);
    OutputDebugStringA(format);
}

#define _msg(txt) MessageBoxA(NULL, (const char *)txt, (const char *)"WC warning", 0x30 | 0x6L | 0x100)

#define SDL_HWSURFACE 1
#define SDL_FULLSCREEN 1
#define SDL_DOUBLEBUF 2
#include <stdio.h>
#ifndef SDL1
#define SDL_SRCCOLORKEY 1
#endif

typedef unsigned int Uint32;

#endif