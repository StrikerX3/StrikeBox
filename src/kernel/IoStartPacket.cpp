#include "common.h"

/*
 * IoStartPacket
 *
 * Import Number:      83
 * Calling Convention: stdcall
 * Parameter 0:        PDEVICE_OBJECT DeviceObject
 * Parameter 1:        PIRP           Irp
 * Parameter 2:        PULONG         Key
 * Return Type:        VOID
 */
int Xbox::IoStartPacket()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PDEVICE_OBJECT, DeviceObject);
	K_INIT_ARG(XboxTypes::PIRP,           Irp);
	K_INIT_ARG(XboxTypes::PULONG,         Key);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
