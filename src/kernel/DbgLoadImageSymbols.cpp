#include "common.h"

/*
 * DbgLoadImageSymbols
 *
 * Import Number:      7
 * Calling Convention: stdcall
 * Parameter 0:        PSTRING   FileName
 * Parameter 1:        PVOID     ImageBase
 * Parameter 2:        ULONG_PTR ProcessId
 * Return Type:        VOID
 */
int Xbox::DbgLoadImageSymbols()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PSTRING,   FileName);
	K_INIT_ARG(XboxTypes::PVOID,     ImageBase);
	K_INIT_ARG(XboxTypes::ULONG_PTR, ProcessId);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
