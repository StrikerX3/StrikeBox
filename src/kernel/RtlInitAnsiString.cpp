#include "common.h"

/*
 * RtlInitAnsiString
 *
 * Import Number:      289
 * Calling Convention: stdcall
 * Parameter 0:        PANSI_STRING DestinationString
 * Parameter 1:        PCSZ         SourceString
 * Return Type:        VOID
 */
int Xbox::RtlInitAnsiString()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PANSI_STRING, DestinationString);
	K_INIT_ARG(XboxTypes::PCSZ,         SourceString);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
