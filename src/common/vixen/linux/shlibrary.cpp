#if defined(__linux__) || defined(LINUX)

#include "vixen/shlibrary.h"
#include "vixen/log.h"

#include <dlfcn.h>

namespace vixen {

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
    char *pathStr = new char[path.size()+1];
    wcstombs(pathStr, path.c_str(), path.size());

    void *libHandle = ::dlopen(pathStr, RTLD_LAZY);

    delete[] pathStr;
    if (libHandle == nullptr) {
        log_error("%s\n", dlerror());
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

