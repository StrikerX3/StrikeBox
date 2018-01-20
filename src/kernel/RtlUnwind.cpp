#include "common.h"

/*
 * RtlUnwind
 *
 * Import Number:      312
 * Calling Convention: stdcall
 * Parameter 0:        PVOID             TargetFrame
 * Parameter 1:        PVOID             TargetIp
 * Parameter 2:        PEXCEPTION_RECORD ExceptionRecord
 * Parameter 3:        PVOID             ReturnValue
 * Return Type:        VOID
 */
int Xbox::RtlUnwind()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PVOID,             TargetFrame);
	K_INIT_ARG(XboxTypes::PVOID,             TargetIp);
	K_INIT_ARG(XboxTypes::PEXCEPTION_RECORD, ExceptionRecord);
	K_INIT_ARG(XboxTypes::PVOID,             ReturnValue);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
