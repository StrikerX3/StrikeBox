#include "common.h"

/*
 * RtlMapGenericMask
 *
 * Import Number:      297
 * Calling Convention: stdcall
 * Parameter 0:        PACCESS_MASK     AccessMask
 * Parameter 1:        PGENERIC_MAPPING GenericMapping
 * Return Type:        VOID
 */
int Xbox::RtlMapGenericMask()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PACCESS_MASK,     AccessMask);
	K_INIT_ARG(XboxTypes::PGENERIC_MAPPING, GenericMapping);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
