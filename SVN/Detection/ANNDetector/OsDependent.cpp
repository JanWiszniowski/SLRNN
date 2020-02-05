#include "OsDependent.h"
#include <iostream>

#if defined(__GNUC__)
#define wstring string
#endif // defined

std::wstring s2ws(const std::string& s)
{
    #if defined(_MSC_VER)
        int len;
        int slength = (int)s.length() + 1;
        len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
        wchar_t* buf = new wchar_t[len];
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
        std::wstring r(buf);
        delete[] buf;
        return r;
	#elif defined(__GNUC__)
        return s;
	#endif // defined
}

void* loadSharedLibrary(std::string libName, int loadMode) {
	#if defined(_MSC_VER)
		std::wstring stemp = s2ws(libName);
		return (void*)LoadLibrary(stemp.c_str());
	#elif defined(__GNUC__)
        std::cout << "\nLoading " << libName << "\n";
        void* v = dlopen(libName.c_str(), loadMode);
        if(!v) std::cout << "\n\nERROR: " << dlerror() << "\n\n";
		return v;
	#endif
}

void* getLibraryFunction(void* lib, std::string funcName) {
	#if defined(_MSC_VER)
		return (void*)GetProcAddress((HINSTANCE)lib, funcName.c_str());
	#elif defined(__GNUC__)
		return dlsym(lib, funcName.c_str());
	#endif
}

bool freeSharedLibrary(void* libHandler) {
	#if defined(_MSC_VER)
		return FreeLibrary((HINSTANCE)libHandler);
	#elif defined(__GNUC__)
		return dlclose(libHandler);
	#endif
}

