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