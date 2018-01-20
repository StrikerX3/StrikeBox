#include "common.h"

/*
 * WRITE_PORT_BUFFER_USHORT
 *
 * Import Number:      333
 * Calling Convention: stdcall
 * Parameter 0:        PUSHORT Port
 * Parameter 1:        PUSHORT Buffer
 * Parameter 2:        ULONG   Count
 * Return Type:        VOID
 */
int Xbox::WRITE_PORT_BUFFER_USHORT()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PUSHORT, Port);
	K_INIT_ARG(XboxTypes::PUSHORT, Buffer);
	K_INIT_ARG(XboxTypes::ULONG,   Count);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
