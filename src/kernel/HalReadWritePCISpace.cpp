#include "common.h"

/*
 * HalReadWritePCISpace
 *
 * Import Number:      46
 * Calling Convention: stdcall
 * Parameter 0:        ULONG   BusNumber
 * Parameter 1:        ULONG   SlotNumber
 * Parameter 2:        ULONG   RegisterNumber
 * Parameter 3:        PVOID   Buffer
 * Parameter 4:        ULONG   Length
 * Parameter 5:        BOOLEAN WritePCISpace
 * Return Type:        VOID
 */
int Xbox::HalReadWritePCISpace()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG,   BusNumber);
	K_INIT_ARG(XboxTypes::ULONG,   SlotNumber);
	K_INIT_ARG(XboxTypes::ULONG,   RegisterNumber);
	K_INIT_ARG(XboxTypes::PVOID,   Buffer);
	K_INIT_ARG(XboxTypes::ULONG,   Length);
	K_INIT_ARG(XboxTypes::BOOLEAN, WritePCISpace);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
