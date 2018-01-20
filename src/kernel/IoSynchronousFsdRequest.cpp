#include "common.h"

/*
 * IoSynchronousFsdRequest
 *
 * Import Number:      85
 * Calling Convention: stdcall
 * Parameter 0:        ULONG          MajorFunction
 * Parameter 1:        PDEVICE_OBJECT DeviceObject
 * Parameter 2:        PVOID          Buffer
 * Parameter 3:        ULONG          Length
 * Parameter 4:        PLARGE_INTEGER StartingOffset
 * Return Type:        NTSTATUS
 */
int Xbox::IoSynchronousFsdRequest()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG,          MajorFunction);
	K_INIT_ARG(XboxTypes::PDEVICE_OBJECT, DeviceObject);
	K_INIT_ARG(XboxTypes::PVOID,          Buffer);
	K_INIT_ARG(XboxTypes::ULONG,          Length);
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER, StartingOffset);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
