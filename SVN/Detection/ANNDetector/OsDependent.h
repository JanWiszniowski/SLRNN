#ifndef os_dependent_h
#define os_dependent_h
#include <string>
#if defined(_MSC_VER) // Microsoft compiler
	#include <windows.h>
#elif defined(__GNUC__) // GNU compiler
	#include <dlfcn.h>
#else
	#error [osDependent.h] Define your compiler!
#endif

void* loadSharedLibrary(std::string libName, int loadMode = 2);
void* getLibraryFunction(void* lib, std::string funcName);
bool freeSharedLibrary(void* libHandler);

#endif

