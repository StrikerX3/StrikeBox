#include "common.h"

/*
 * IoDismountVolumeByName
 *
 * Import Number:      91
 * Calling Convention: stdcall
 * Parameter 0:        POBJECT_STRING DeviceName
 * Return Type:        NTSTATUS
 */
int Xbox::IoDismountVolumeByName()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::POBJECT_STRING, DeviceName);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
