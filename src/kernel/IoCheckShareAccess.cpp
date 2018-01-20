#include "common.h"

/*
 * IoCheckShareAccess
 *
 * Import Number:      63
 * Calling Convention: stdcall
 * Parameter 0:        ACCESS_MASK   DesiredAccess
 * Parameter 1:        ULONG         DesiredShareAccess
 * Parameter 2:        PFILE_OBJECT  FileObject
 * Parameter 3:        PSHARE_ACCESS ShareAccess
 * Parameter 4:        BOOLEAN       Update
 * Return Type:        NTSTATUS
 */
int Xbox::IoCheckShareAccess()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ACCESS_MASK,   DesiredAccess);
	K_INIT_ARG(XboxTypes::ULONG,         DesiredShareAccess);
	K_INIT_ARG(XboxTypes::PFILE_OBJECT,  FileObject);
	K_INIT_ARG(XboxTypes::PSHARE_ACCESS, ShareAccess);
	K_INIT_ARG(XboxTypes::BOOLEAN,       Update);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
