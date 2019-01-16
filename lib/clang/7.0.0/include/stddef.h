/* stddef.h standard header */
#ifndef _STDDEF
#define _STDDEF
#ifndef _YVALS
#include <yvals.h>
#endif

/* macros */
#define NULL 0
#define offsetof(T, member) ((_Size_t)&((T*)0)->member)

/* type definitions*/
#ifndef _SIZET
#define _SIZET  
typedef unsigned int   size_t;
#endif
#ifndef _WCHART
#define _WCHART char
typedef char  wchar_t;
#endif
typedef unsigned int ptrdiff_t;
#endif
