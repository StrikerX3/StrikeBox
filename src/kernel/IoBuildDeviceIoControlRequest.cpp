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
	K_INIT_ARG(XboxTypes::ULONG,            IoControlCode);
	K_INIT_ARG(XboxTypes::PDEVICE_OBJECT,   DeviceObject);
	K_INIT_ARG(XboxTypes::PVOID,            InputBuffer);
	K_INIT_ARG(XboxTypes::ULONG,            InputBufferLength);
	K_INIT_ARG(XboxTypes::PVOID,            OutputBuffer);
	K_INIT_ARG(XboxTypes::ULONG,            OutputBufferLength);
	K_INIT_ARG(XboxTypes::BOOLEAN,          InternalDeviceIoControl);
	K_INIT_ARG(XboxTypes::PKEVENT,          Event);
	K_INIT_ARG(XboxTypes::PIO_STATUS_BLOCK, IoStatusBlock);
	XboxTypes::PIRP rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
