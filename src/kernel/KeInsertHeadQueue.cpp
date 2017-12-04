#include "common.h"

/*
 * KeInsertHeadQueue
 *
 * Import Number:      116
 * Calling Convention: stdcall
 * Parameter 0:        PRKQUEUE    Queue
 * Parameter 1:        PLIST_ENTRY Entry
 * Return Type:        LONG
 */
int Xbox::KeInsertHeadQueue()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PRKQUEUE,    Queue);
	K_INIT_ARG(PLIST_ENTRY, Entry);
	LONG rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
