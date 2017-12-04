#include "common.h"

/*
 * KeRemoveEntryDeviceQueue
 *
 * Import Number:      135
 * Calling Convention: stdcall
 * Parameter 0:        PKDEVICE_QUEUE DeviceQueue
 * Parameter 1:        PKDEVICE_QUEUE DeviceQueueEntry
 * Return Type:        BOOLEAN
 */
int Xbox::KeRemoveEntryDeviceQueue()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKDEVICE_QUEUE, DeviceQueue);
	K_INIT_ARG(PKDEVICE_QUEUE, DeviceQueueEntry);
	BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
