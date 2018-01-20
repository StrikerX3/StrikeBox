#include "common.h"

/*
 * WRITE_PORT_BUFFER_ULONG
 *
 * Import Number:      334
 * Calling Convention: stdcall
 * Parameter 0:        PULONG Port
 * Parameter 1:        PULONG Buffer
 * Parameter 2:        ULONG  Count
 * Return Type:        VOID
 */
int Xbox::WRITE_PORT_BUFFER_ULONG()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PULONG, Port);
	K_INIT_ARG(XboxTypes::PULONG, Buffer);
	K_INIT_ARG(XboxTypes::ULONG,  Count);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
