#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <xbox.h>

#define ERROR_NOT_IMPLEMENTED -1

#define K_ENTER_CDECL() \
    CdeclHelper cc(m_cpu);

#define K_ENTER_FASTCALL() \
    FastcallHelper cc(m_cpu);

#define K_ENTER_STDCALL() \
    StdcallHelper cc(m_cpu);

// Get the argument from the stack frame according to the calling convention
#define K_INIT_ARG(TYPE, NAME) \
	TYPE NAME; \
	cc.GetArgument(&NAME, sizeof(TYPE));

// Return from the kernel function
#define K_EXIT() \
    do { \
        m_cpu->RegWrite(REG_EIP, cc.GetReturnAddress()); \
        cc.Cleanup(); \
    } while (0)

// Return from the kernel function with a value
#define K_EXIT_WITH_VALUE(VAL) \
    do { \
        cc.SetReturnValue(&VAL, sizeof(VAL)); \
        K_EXIT(); \
    } while (0)

#endif
