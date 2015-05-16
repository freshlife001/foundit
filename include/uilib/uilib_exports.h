#pragma once


#if defined(_WIN32) && defined(_WINDLL)
# ifdef UILIB_EXPORTS
#  define UILIB_EXPORT __declspec(dllexport)
# else
#  define UILIB_EXPORT __declspec(dllimport)
# endif
#else
# define UILIB_EXPORT
#endif

