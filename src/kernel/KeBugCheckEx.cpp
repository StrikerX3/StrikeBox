#include "common.h"

/*
 * KeBugCheckEx
 *
 * Import Number:      96
 * Calling Convention: stdcall
 * Parameter 0:        ULONG     BugCheckCode
 * Parameter 1:        ULONG_PTR BugCheckParameter1
 * Parameter 2:        ULONG_PTR BugCheckParameter2
 * Parameter 3:        ULONG_PTR BugCheckParameter3
 * Parameter 4:        ULONG_PTR BugCheckParameter4
 * Return Type:        VOID
 */
int Xbox::KeBugCheckEx()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::ULONG,     BugCheckCode);
	K_INIT_ARG(XboxTypes::ULONG_PTR, BugCheckParameter1);
	K_INIT_ARG(XboxTypes::ULONG_PTR, BugCheckParameter2);
	K_INIT_ARG(XboxTypes::ULONG_PTR, BugCheckParameter3);
	K_INIT_ARG(XboxTypes::ULONG_PTR, BugCheckParameter4);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
