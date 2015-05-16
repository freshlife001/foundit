#pragma once


#if defined(_WIN32) && defined(_WINDLL)
# ifdef CORE_EXPORTS
#  define LIBCORE_EXPORT __declspec(dllexport)
# else
#  define LIBCORE_EXPORT __declspec(dllimport)
# endif
#else
# define LIBCORE_EXPORT
#endif

