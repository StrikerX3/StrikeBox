#include "common.h"

/*
 * HalWriteSMBusValue
 *
 * Import Number:      50
 * Calling Convention: stdcall
 * Parameter 0:        UCHAR   SlaveAddress
 * Parameter 1:        UCHAR   CommandCode
 * Parameter 2:        BOOLEAN WriteWordValue
 * Parameter 3:        ULONG   DataValue
 * Return Type:        NTSTATUS
 */
int Xbox::HalWriteSMBusValue()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::UCHAR,   SlaveAddress);
	K_INIT_ARG(XboxTypes::UCHAR,   CommandCode);
	K_INIT_ARG(XboxTypes::BOOLEAN, WriteWordValue);
	K_INIT_ARG(XboxTypes::ULONG,   DataValue);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
