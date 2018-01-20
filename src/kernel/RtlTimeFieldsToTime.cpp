#include "common.h"

/*
 * RtlTimeFieldsToTime
 *
 * Import Number:      304
 * Calling Convention: stdcall
 * Parameter 0:        PTIME_FIELDS   TimeFields
 * Parameter 1:        PLARGE_INTEGER Time
 * Return Type:        BOOLEAN
 */
int Xbox::RtlTimeFieldsToTime()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PTIME_FIELDS,   TimeFields);
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER, Time);
	XboxTypes::BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
