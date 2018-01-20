#include "common.h"

/*
 * IoBuildSynchronousFsdRequest
 *
 * Import Number:      62
 * Calling Convention: stdcall
 * Parameter 0:        ULONG            MajorFunction
 * Parameter 1:        PDEVICE_OBJECT   DeviceObject
 * Parameter 2:        PVOID            Buffer
 * Parameter 3:        ULONG            Length
 * Parameter 4:        PLARGE_INTEGER   StartingOffset
 * Parameter 5:        PKEVENT          Event
 * Parameter 6:        PIO_STATUS_BLOCK IoStatusBlock
 * Return Type:        PIRP
 */
int Xbox::IoBuildSynchronousFsdRequest()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG,            MajorFunction);
	K_INIT_ARG(XboxTypes::PDEVICE_OBJECT,   DeviceObject);
	K_INIT_ARG(XboxTypes::PVOID,            Buffer);
	K_INIT_ARG(XboxTypes::ULONG,            Length);
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER,   StartingOffset);
	K_INIT_ARG(XboxTypes::PKEVENT,          Event);
	K_INIT_ARG(XboxTypes::PIO_STATUS_BLOCK, IoStatusBlock);
	XboxTypes::PIRP rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
