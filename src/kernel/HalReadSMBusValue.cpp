#include "common.h"

/*
 * HalReadSMBusValue
 *
 * Import Number:      45
 * Calling Convention: stdcall
 * Parameter 0:        UCHAR   SlaveAddress
 * Parameter 1:        UCHAR   CommandCode
 * Parameter 2:        BOOLEAN ReadWordValue
 * Parameter 3:        PULONG  DataValue
 * Return Type:        NTSTATUS
 */
int Xbox::HalReadSMBusValue()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(UCHAR,   SlaveAddress);
	K_INIT_ARG(UCHAR,   CommandCode);
	K_INIT_ARG(BOOLEAN, ReadWordValue);
	K_INIT_ARG(PULONG,  DataValue);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
