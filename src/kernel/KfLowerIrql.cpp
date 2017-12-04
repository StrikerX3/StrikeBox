#include "common.h"

/*
 * KfLowerIrql
 *
 * Import Number:      161
 * Calling Convention: fastcall
 * Parameter 0:        KIRQL NewIrql
 * Return Type:        VOID
 */
int Xbox::KfLowerIrql()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(KIRQL, NewIrql);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
