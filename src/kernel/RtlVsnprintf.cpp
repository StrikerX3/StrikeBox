#include "common.h"

/*
 * RtlVsnprintf
 *
 * Import Number:      363
 * Calling Convention: cdecl
 * Parameter 0:        PCHAR  pszDest
 * Parameter 1:        SIZE_T cbDest
 * Parameter 2:        LPCSTR pszFormat
 * Return Type:        VOID
 */
int Xbox::RtlVsnprintf()
{
	K_ENTER_CDECL();
	K_INIT_ARG(PCHAR,  pszDest);
	K_INIT_ARG(SIZE_T, cbDest);
	K_INIT_ARG(LPCSTR, pszFormat);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
