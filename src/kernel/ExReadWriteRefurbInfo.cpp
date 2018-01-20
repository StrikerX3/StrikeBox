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
	K_INIT_ARG(XboxTypes::PXBOX_REFURB_INFO, RefurbInfo);
	K_INIT_ARG(XboxTypes::ULONG,             ValueLength);
	K_INIT_ARG(XboxTypes::BOOLEAN,           DoWrite);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
