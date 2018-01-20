#include "common.h"

/*
 * KeInsertDeviceQueue
 *
 * Import Number:      115
 * Calling Convention: stdcall
 * Parameter 0:        PKDEVICE_QUEUE       DeviceQueue
 * Parameter 1:        PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
 * Return Type:        BOOLEAN
 */
int Xbox::KeInsertDeviceQueue()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PKDEVICE_QUEUE,       DeviceQueue);
	K_INIT_ARG(XboxTypes::PKDEVICE_QUEUE_ENTRY, DeviceQueueEntry);
	XboxTypes::BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
