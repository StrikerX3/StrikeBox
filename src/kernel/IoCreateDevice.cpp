#include "common.h"

/*
 * IoCreateDevice
 *
 * Import Number:      65
 * Calling Convention: stdcall
 * Parameter 0:        PDRIVER_OBJECT  DriverObject
 * Parameter 1:        ULONG           DeviceExtensionSize
 * Parameter 2:        POBJECT_STRING  DeviceName
 * Parameter 3:        DEVICE_TYPE     DeviceType
 * Parameter 4:        BOOLEAN         Exclusive
 * Parameter 5:        PPDEVICE_OBJECT DeviceObject
 * Return Type:        NTSTATUS
 */
int Xbox::IoCreateDevice()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PDRIVER_OBJECT,  DriverObject);
	K_INIT_ARG(ULONG,           DeviceExtensionSize);
	K_INIT_ARG(POBJECT_STRING,  DeviceName);
	K_INIT_ARG(DEVICE_TYPE,     DeviceType);
	K_INIT_ARG(BOOLEAN,         Exclusive);
	K_INIT_ARG(PPDEVICE_OBJECT, DeviceObject);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
