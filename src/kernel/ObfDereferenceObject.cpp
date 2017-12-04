#include "common.h"

/*
 * ObfDereferenceObject
 *
 * Import Number:      250
 * Calling Convention: fastcall
 * Parameter 0:        PVOID Object
 * Return Type:        VOID
 */
int Xbox::ObfDereferenceObject()
{
	K_ENTER_FASTCALL();
	K_INIT_ARG(PVOID, Object);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
