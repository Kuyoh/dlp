#pragma once

#if defined(BUILD_DLPRT_API) && defined(HAVE_VISIBILITY)
#define DLPRT_API __attribute__((__visibility__("default")))
#elif defined(BUILD_DLPRT_API) && defined(_MSC_VER)
#define DLPRT_API __declspec(dllexport)
#elif defined(_MSC_VER)
#define DLPRT_API __declspec(dllimport)
#else
#define DLPRT_API
#endif

// TODO: we need to figure out a way to free the library even when the dll is still loaded (maybe via thread_attach/detach?)
extern "C" DLPRT_API void *findDynSymbol(const char *libName, const char *symName);

