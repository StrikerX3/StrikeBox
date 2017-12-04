#include "common.h"

/*
 * RtlVsprintf
 *
 * Import Number:      364
 * Calling Convention: cdecl
 * Parameter 0:        PCHAR  pszDest
 * Parameter 1:        LPCSTR pszFormat
 * Return Type:        VOID
 */
int Xbox::RtlVsprintf()
{
	K_ENTER_CDECL();
	K_INIT_ARG(PCHAR,  pszDest);
	K_INIT_ARG(LPCSTR, pszFormat);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
