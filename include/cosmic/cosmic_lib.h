#ifndef COSMIC_LIB_H
#define COSMIC_LIB_H

#ifdef _WIN32
#ifdef COSMIC_DLL_EXPORT
#define COSMIC_DLL __declspec(dllexport)
#elif defined(COSMIC_DEVELOP)
#define COSMIC_DLL
#else
#define COSMIC_DLL __declspec(dllimport)
#endif
#else
#define COSMIC_DLL
#endif

#endif