#include "common.h"

/*
 * KiUnlockDispatcherDatabase
 *
 * Import Number:      163
 * Calling Convention: fastcall
 * Parameter 0:        KIRQL OldIrql
 * Return Type:        VOID
 */
int Xbox::KiUnlockDispatcherDatabase()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(XboxTypes::KIRQL, OldIrql);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
