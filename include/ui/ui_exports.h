#pragma once


#if defined(_WIN32) && defined(_WINDLL)
# ifdef UI_EXPORTS
#  define LIBUI_EXPORT __declspec(dllexport)
# else
#  define LIBUI_EXPORT __declspec(dllimport)
# endif
#else
# define LIBUI_EXPORT
#endif

