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
	K_INIT_ARG(XboxTypes::PCHAR,  pszDest);
	K_INIT_ARG(XboxTypes::SIZE_T, cbDest);
	K_INIT_ARG(XboxTypes::LPCSTR, pszFormat);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
