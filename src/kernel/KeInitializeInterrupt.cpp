#include "common.h"

/*
 * KeInitializeInterrupt
 *
 * Import Number:      109
 * Calling Convention: stdcall
 * Parameter 0:        PKINTERRUPT       Interrupt
 * Parameter 1:        PKSERVICE_ROUTINE ServiceRoutine
 * Parameter 2:        PVOID             ServiceContext
 * Parameter 3:        ULONG             Vector
 * Parameter 4:        KIRQL             Irql
 * Parameter 5:        KINTERRUPT_MODE   InterruptMode
 * Parameter 6:        BOOLEAN           ShareVector
 * Return Type:        VOID
 */
int Xbox::KeInitializeInterrupt()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PKINTERRUPT,       Interrupt);
	K_INIT_ARG(PKSERVICE_ROUTINE, ServiceRoutine);
	K_INIT_ARG(PVOID,             ServiceContext);
	K_INIT_ARG(ULONG,             Vector);
	K_INIT_ARG(KIRQL,             Irql);
	K_INIT_ARG(KINTERRUPT_MODE,   InterruptMode);
	K_INIT_ARG(BOOLEAN,           ShareVector);

	K_EXIT();
	return ERROR_NOT_IMPLEMENTED;
}
