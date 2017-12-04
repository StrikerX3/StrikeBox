#include "common.h"

/*
 * HalReturnToFirmware
 *
 * Import Number:      49
 * Calling Convention: stdcall
 * Parameter 0:        FIRMWARE_REENTRY Routine
 * Return Type:        VOID
 */
int Xbox::HalReturnToFirmware()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(FIRMWARE_REENTRY, Routine);

	m_should_run = false;

	K_EXIT();
	return 0;
}
