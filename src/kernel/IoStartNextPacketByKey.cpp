#include "common.h"

/*
 * IoStartNextPacketByKey
 *
 * Import Number:      82
 * Calling Convention: stdcall
 * Parameter 0:        PDEVICE_OBJECT DeviceObject
 * Parameter 1:        ULONG          Key
 * Return Type:        VOID
 */
int Xbox::IoStartNextPacketByKey()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PDEVICE_OBJECT, DeviceObject);
	K_INIT_ARG(ULONG,          Key);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
