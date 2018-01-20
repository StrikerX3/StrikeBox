#include <stdio.h>
#include <string.h>
#include "xbe.h"
#include "xbox.h"

// Debugging stuff
#define DUMP_XBE_INFO 0

#ifdef _WIN32
#undef main

char *basename(char *path)
{
	char name[MAX_PATH];
	char ext[MAX_PATH];
	if (_splitpath_s(path, NULL, 0, NULL, 0, name, MAX_PATH, ext, MAX_PATH)) {
		return NULL;
	}
	int len = strlen(name) + strlen(ext) + 1;
	char *out = (char *)malloc(len);
	strcpy(out, name);
	strcat(out, ext);
	return out;
}
#endif

/*!
 * Program entry point
 */
int main(int argc, const char *argv[])
{
    const char *xbe_path;
    const char *usage = "usage: %s <xbe>\n";

    if (argc != 2) {
        printf(usage, basename((char*)argv[0]));
        return 1;
    }

    // Load XBE executable
    xbe_path = argv[1];
    Xbe *xbe = new Xbe(xbe_path);
    if (xbe->GetError() != 0) {
        return 1;
    }

#if DUMP_XBE_INFO
    xbe->DumpInformation(stdout);
#endif

    Xbox *xbox = new Xbox();
    xbox->Initialize();
    xbox->LoadXbe(xbe);
    xbox->Run();

    delete xbox;
    delete xbe;
    return 0;
}
