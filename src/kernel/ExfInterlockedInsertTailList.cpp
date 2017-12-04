#include "common.h"

/*
 * ExfInterlockedInsertTailList
 *
 * Import Number:      33
 * Calling Convention: fastcall
 * Parameter 0:        PLIST_ENTRY ListHead
 * Parameter 1:        PLIST_ENTRY ListEntry
 * Return Type:        PLIST_ENTRY
 */
int Xbox::ExfInterlockedInsertTailList()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(PLIST_ENTRY, ListHead);
	K_INIT_ARG(PLIST_ENTRY, ListEntry);
	PLIST_ENTRY rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
