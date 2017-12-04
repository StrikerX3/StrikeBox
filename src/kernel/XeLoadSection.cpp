#include "common.h"

/*
 * XeLoadSection
 *
 * Import Number:      327
 * Calling Convention: stdcall
 * Parameter 0:        PXBEIMAGE_SECTION Section
 * Return Type:        NTSTATUS
 */
int Xbox::XeLoadSection()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PXBEIMAGE_SECTION, Section);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
