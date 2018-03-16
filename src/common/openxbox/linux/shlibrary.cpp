#if defined(__linux__) || defined(LINUX)

#include "openxbox/shlibrary.h"

#include <dlfcn.h>

namespace openxbox {

class LinuxSharedLibrary : public SharedLibrary {
public:
    // TODO: implement
    LinuxSharedLibrary(void *libHandle);
    ~LinuxSharedLibrary();

    void *GetExport(std::string exportName);
private:
    void *m_libHandle;
};

SharedLibraryStatus SharedLibrary_Load(std::wstring path, SharedLibrary **library) {
    char *pathStr = new char[path.size()];
    wcstombs(pathStr, path.c_str(), path.size());

    void *libHandle = ::dlopen(pathStr, RTLD_LAZY);
    delete[] pathStr;
    if (libHandle == nullptr) {
        return kLibraryUnspecifiedError;
    }
    *library = new LinuxSharedLibrary(libHandle);
    return kLibraryLoadSuccess;
}

LinuxSharedLibrary::LinuxSharedLibrary(void *libHandle)
    : m_libHandle(libHandle)
{
}

LinuxSharedLibrary::~LinuxSharedLibrary() {
}

void *LinuxSharedLibrary::GetExport(std::string exportName) {
    // The correct way to check if the symbols exists and is not just NULL is
    // to execute dlerror(), then save the result of dlsym() to a variable,
    // then check if dlerror() is NULL. If it is, the symbol does not exist.

    // However, since GetExport doesn't care about the existence of the symbol,
    // we simply return the result of dlsym().
    return ::dlsym(m_libHandle, exportName.c_str());
}

}

#endif // LINUX

