#include "common.h"

/*
 * IoDismountVolume
 *
 * Import Number:      90
 * Calling Convention: stdcall
 * Parameter 0:        PDEVICE_OBJECT DeviceObject
 * Return Type:        NTSTATUS
 */
int Xbox::IoDismountVolume()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PDEVICE_OBJECT, DeviceObject);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
