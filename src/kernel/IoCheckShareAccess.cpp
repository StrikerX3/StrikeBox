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
	K_INIT_ARG(ACCESS_MASK,   DesiredAccess);
	K_INIT_ARG(ULONG,         DesiredShareAccess);
	K_INIT_ARG(PFILE_OBJECT,  FileObject);
	K_INIT_ARG(PSHARE_ACCESS, ShareAccess);
	K_INIT_ARG(BOOLEAN,       Update);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
