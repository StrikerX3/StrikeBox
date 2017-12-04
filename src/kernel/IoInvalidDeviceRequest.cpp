#include "common.h"

/*
 * IoInvalidDeviceRequest
 *
 * Import Number:      74
 * Calling Convention: stdcall
 * Parameter 0:        PDEVICE_OBJECT DeviceObject
 * Parameter 1:        PIRP           Irp
 * Return Type:        NTSTATUS
 */
int Xbox::IoInvalidDeviceRequest()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PDEVICE_OBJECT, DeviceObject);
	K_INIT_ARG(PIRP,           Irp);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
