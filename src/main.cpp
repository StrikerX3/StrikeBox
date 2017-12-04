#include <stdio.h>
#include <string.h>
#include "xbe.h"
#include "xbox.h"

// Debugging stuff
#define DUMP_XBE_INFO 0

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
