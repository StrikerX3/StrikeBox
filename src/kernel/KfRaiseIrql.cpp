#include "common.h"

/*
 * KfRaiseIrql
 *
 * Import Number:      160
 * Calling Convention: fastcall
 * Parameter 0:        KIRQL NewIrql
 * Return Type:        KIRQL
 */
int Xbox::KfRaiseIrql()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(XboxTypes::KIRQL, NewIrql);
	XboxTypes::KIRQL rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
