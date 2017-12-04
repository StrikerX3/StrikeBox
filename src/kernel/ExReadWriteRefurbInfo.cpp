#include "common.h"

/*
 * ExReadWriteRefurbInfo
 *
 * Import Number:      25
 * Calling Convention: stdcall
 * Parameter 0:        PXBOX_REFURB_INFO RefurbInfo
 * Parameter 1:        ULONG             ValueLength
 * Parameter 2:        BOOLEAN           DoWrite
 * Return Type:        NTSTATUS
 */
int Xbox::ExReadWriteRefurbInfo()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PXBOX_REFURB_INFO, RefurbInfo);
	K_INIT_ARG(ULONG,             ValueLength);
	K_INIT_ARG(BOOLEAN,           DoWrite);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
