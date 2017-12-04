#include "common.h"

/*
 * InterlockedPopEntrySList
 *
 * Import Number:      57
 * Calling Convention: fastcall
 * Parameter 0:        PSLIST_HEADER ListHead
 * Return Type:        PSINGLE_LIST_ENTRY
 */
int Xbox::InterlockedPopEntrySList()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(PSLIST_HEADER, ListHead);
	PSINGLE_LIST_ENTRY rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
