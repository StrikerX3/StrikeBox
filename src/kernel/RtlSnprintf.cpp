#include "common.h"

/*
 * RtlSnprintf
 *
 * Import Number:      361
 * Calling Convention: cdecl
 * Parameter 0:        PCHAR  pszDest
 * Parameter 1:        SIZE_T cbDest
 * Parameter 2:        LPCSTR pszFormat
 * Return Type:        VOID
 */
int Xbox::RtlSnprintf()
{
	K_ENTER_CDECL();
	K_INIT_ARG(XboxTypes::PCHAR,  pszDest);
	K_INIT_ARG(XboxTypes::SIZE_T, cbDest);
	K_INIT_ARG(XboxTypes::LPCSTR, pszFormat);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
