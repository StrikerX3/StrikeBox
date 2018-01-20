#include "common.h"

/*
 * HalRegisterShutdownNotification
 *
 * Import Number:      47
 * Calling Convention: stdcall
 * Parameter 0:        PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration
 * Parameter 1:        BOOLEAN                    Register
 * Return Type:        VOID
 */
int Xbox::HalRegisterShutdownNotification()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PHAL_SHUTDOWN_REGISTRATION, ShutdownRegistration);
	K_INIT_ARG(XboxTypes::BOOLEAN,                    Register);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
