#include "common.h"

/*
 * IoBuildDeviceIoControlRequest
 *
 * Import Number:      61
 * Calling Convention: stdcall
 * Parameter 0:        ULONG            IoControlCode
 * Parameter 1:        PDEVICE_OBJECT   DeviceObject
 * Parameter 2:        PVOID            InputBuffer
 * Parameter 3:        ULONG            InputBufferLength
 * Parameter 4:        PVOID            OutputBuffer
 * Parameter 5:        ULONG            OutputBufferLength
 * Parameter 6:        BOOLEAN          InternalDeviceIoControl
 * Parameter 7:        PKEVENT          Event
 * Parameter 8:        PIO_STATUS_BLOCK IoStatusBlock
 * Return Type:        PIRP
 */
int Xbox::IoBuildDeviceIoControlRequest()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(ULONG,            IoControlCode);
	K_INIT_ARG(PDEVICE_OBJECT,   DeviceObject);
	K_INIT_ARG(PVOID,            InputBuffer);
	K_INIT_ARG(ULONG,            InputBufferLength);
	K_INIT_ARG(PVOID,            OutputBuffer);
	K_INIT_ARG(ULONG,            OutputBufferLength);
	K_INIT_ARG(BOOLEAN,          InternalDeviceIoControl);
	K_INIT_ARG(PKEVENT,          Event);
	K_INIT_ARG(PIO_STATUS_BLOCK, IoStatusBlock);
	PIRP rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
