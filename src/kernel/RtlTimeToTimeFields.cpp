#include "common.h"

/*
 * RtlTimeToTimeFields
 *
 * Import Number:      305
 * Calling Convention: stdcall
 * Parameter 0:        PLARGE_INTEGER Time
 * Parameter 1:        PTIME_FIELDS   TimeFields
 * Return Type:        VOID
 */
int Xbox::RtlTimeToTimeFields()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER, Time);
	K_INIT_ARG(XboxTypes::PTIME_FIELDS,   TimeFields);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
