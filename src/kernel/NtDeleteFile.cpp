#include "common.h"

/*
 * NtDeleteFile
 *
 * Import Number:      195
 * Calling Convention: stdcall
 * Parameter 0:        POBJECT_ATTRIBUTES ObjectAttributes
 * Return Type:        BOOLEAN
 */
int Xbox::NtDeleteFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::POBJECT_ATTRIBUTES, ObjectAttributes);
	XboxTypes::BOOLEAN rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
