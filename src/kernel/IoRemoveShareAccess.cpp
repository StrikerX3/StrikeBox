#include "common.h"

/*
 * IoRemoveShareAccess
 *
 * Import Number:      78
 * Calling Convention: stdcall
 * Parameter 0:        PFILE_OBJECT  FileObject
 * Parameter 1:        PSHARE_ACCESS ShareAccess
 * Return Type:        VOID
 */
int Xbox::IoRemoveShareAccess()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PFILE_OBJECT,  FileObject);
	K_INIT_ARG(PSHARE_ACCESS, ShareAccess);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
