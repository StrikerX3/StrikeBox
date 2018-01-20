#include "common.h"

/*
 * NtQueryFullAttributesFile
 *
 * Import Number:      210
 * Calling Convention: stdcall
 * Parameter 0:        POBJECT_ATTRIBUTES             ObjectAttributes
 * Parameter 1:        PFILE_NETWORK_OPEN_INFORMATION FileInformation
 * Return Type:        NTSTATUS
 */
int Xbox::NtQueryFullAttributesFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::POBJECT_ATTRIBUTES,             ObjectAttributes);
	K_INIT_ARG(XboxTypes::PFILE_NETWORK_OPEN_INFORMATION, FileInformation);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
