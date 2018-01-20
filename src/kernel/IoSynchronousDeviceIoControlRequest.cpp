#include "common.h"

/*
 * IoSynchronousDeviceIoControlRequest
 *
 * Import Number:      84
 * Calling Convention: stdcall
 * Parameter 0:        ULONG          IoControlCode
 * Parameter 1:        PDEVICE_OBJECT DeviceObject
 * Parameter 2:        PVOID          InputBuffer
 * Parameter 3:        ULONG          InputBufferLength
 * Parameter 4:        PVOID          OutputBuffer
 * Parameter 5:        ULONG          OutputBufferLength
 * Parameter 6:        PULONG         ReturnedOutputBufferLength
 * Parameter 7:        BOOLEAN        InternalDeviceIoControl
 * Return Type:        NTSTATUS
 */
int Xbox::IoSynchronousDeviceIoControlRequest()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG,          IoControlCode);
	K_INIT_ARG(XboxTypes::PDEVICE_OBJECT, DeviceObject);
	K_INIT_ARG(XboxTypes::PVOID,          InputBuffer);
	K_INIT_ARG(XboxTypes::ULONG,          InputBufferLength);
	K_INIT_ARG(XboxTypes::PVOID,          OutputBuffer);
	K_INIT_ARG(XboxTypes::ULONG,          OutputBufferLength);
	K_INIT_ARG(XboxTypes::PULONG,         ReturnedOutputBufferLength);
	K_INIT_ARG(XboxTypes::BOOLEAN,        InternalDeviceIoControl);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
