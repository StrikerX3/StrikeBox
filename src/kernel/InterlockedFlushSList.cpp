#include "common.h"

/*
 * InterlockedFlushSList
 *
 * Import Number:      56
 * Calling Convention: fastcall
 * Parameter 0:        PSLIST_HEADER ListHead
 * Return Type:        PSINGLE_LIST_ENTRY
 */
int Xbox::InterlockedFlushSList()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(PSLIST_HEADER, ListHead);
	PSINGLE_LIST_ENTRY rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
