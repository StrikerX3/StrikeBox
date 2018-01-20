#include "common.h"

/*
 * READ_PORT_BUFFER_UCHAR
 *
 * Import Number:      329
 * Calling Convention: stdcall
 * Parameter 0:        PUCHAR Port
 * Parameter 1:        PUCHAR Buffer
 * Parameter 2:        ULONG  Count
 * Return Type:        VOID
 */
int Xbox::READ_PORT_BUFFER_UCHAR()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PUCHAR, Port);
	K_INIT_ARG(XboxTypes::PUCHAR, Buffer);
	K_INIT_ARG(XboxTypes::ULONG,  Count);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
