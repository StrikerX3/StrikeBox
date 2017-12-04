#include "common.h"

/*
 * KeRemoveByKeyDeviceQueue
 *
 * Import Number:      133
 * Calling Convention: stdcall
 * Parameter 0:        PKDEVICE_QUEUE DeviceQueue
 * Parameter 1:        ULONG          SortKey
 * Return Type:        PKDEVICE_QUEUE_ENTRY
 */
int Xbox::KeRemoveByKeyDeviceQueue()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKDEVICE_QUEUE, DeviceQueue);
	K_INIT_ARG(ULONG,          SortKey);
	PKDEVICE_QUEUE_ENTRY rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
