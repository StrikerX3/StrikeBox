#include "common.h"

/*
 * KeRemoveDeviceQueue
 *
 * Import Number:      134
 * Calling Convention: stdcall
 * Parameter 0:        PKDEVICE_QUEUE DeviceQueue
 * Return Type:        PKDEVICE_QUEUE_ENTRY
 */
int Xbox::KeRemoveDeviceQueue()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKDEVICE_QUEUE, DeviceQueue);
	PKDEVICE_QUEUE_ENTRY rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
