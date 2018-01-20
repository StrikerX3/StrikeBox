#include "common.h"

/*
 * KeInitializeEvent
 *
 * Import Number:      108
 * Calling Convention: stdcall
 * Parameter 0:        PRKEVENT   Event
 * Parameter 1:        EVENT_TYPE Type
 * Parameter 2:        BOOLEAN    State
 * Return Type:        VOID
 */
int Xbox::KeInitializeEvent()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PRKEVENT,   Event);
	K_INIT_ARG(XboxTypes::EVENT_TYPE, Type);
	K_INIT_ARG(XboxTypes::BOOLEAN,    State);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
