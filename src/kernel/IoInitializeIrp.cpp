#include "common.h"

/*
 * IoInitializeIrp
 *
 * Import Number:      73
 * Calling Convention: stdcall
 * Parameter 0:        PIRP   Irp
 * Parameter 1:        USHORT PacketSize
 * Parameter 2:        CCHAR  StackSize
 * Return Type:        VOID
 */
int Xbox::IoInitializeIrp()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PIRP,   Irp);
	K_INIT_ARG(XboxTypes::USHORT, PacketSize);
	K_INIT_ARG(XboxTypes::CCHAR,  StackSize);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
