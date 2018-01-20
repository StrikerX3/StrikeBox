#include "common.h"

/*
 * RtlSprintf
 *
 * Import Number:      362
 * Calling Convention: cdecl
 * Parameter 0:        PCHAR  pszDest
 * Parameter 1:        LPCSTR pszFormat
 * Return Type:        VOID
 */
int Xbox::RtlSprintf()
{
	K_ENTER_CDECL();
	K_INIT_ARG(XboxTypes::PCHAR,  pszDest);
	K_INIT_ARG(XboxTypes::LPCSTR, pszFormat);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
