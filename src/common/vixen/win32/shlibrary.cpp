#ifdef _WIN32

#include "vixen/shlibrary.h"

#include <Windows.h>

namespace vixen {

class Win32SharedLibrary : public SharedLibrary {
public:
	Win32SharedLibrary(HMODULE hm);
	~Win32SharedLibrary();

	void *GetExport(std::string exportName);
private:
	HMODULE m_hm;
};

SharedLibraryStatus SharedLibrary_Load(std::wstring path, SharedLibrary **library) {
	HMODULE hm = LoadLibraryW(path.c_str());
	if (hm == NULL) {
		DWORD lastError = GetLastError();
		switch (lastError) {
		case ERROR_MOD_NOT_FOUND:
			return kLibraryNotFound;
		case ERROR_BAD_EXE_FORMAT:
			return kLibraryInvalidImage;
		default:
			return kLibraryUnspecifiedError;
		}
	}

	*library = new Win32SharedLibrary(hm);
	return kLibraryLoadSuccess;
}

Win32SharedLibrary::Win32SharedLibrary(HMODULE hm)
	: m_hm(hm)
{
}

Win32SharedLibrary::~Win32SharedLibrary() {
	FreeLibrary(m_hm);
}

void *Win32SharedLibrary::GetExport(std::string exportName) {
	return GetProcAddress(m_hm, exportName.c_str());
}

}

#endif
