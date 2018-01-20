#include "common.h"

/*
 * NtDeviceIoControlFile
 *
 * Import Number:      196
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE           FileHandle
 * Parameter 1:        HANDLE           Event
 * Parameter 2:        PIO_APC_ROUTINE  ApcRoutine
 * Parameter 3:        PVOID            ApcContext
 * Parameter 4:        PIO_STATUS_BLOCK IoStatusBlock
 * Parameter 5:        ULONG            IoControlCode
 * Parameter 6:        PVOID            InputBuffer
 * Parameter 7:        ULONG            InputBufferLength
 * Parameter 8:        PVOID            OutputBuffer
 * Parameter 9:        ULONG            OutputBufferLength
 * Return Type:        NTSTATUS
 */
int Xbox::NtDeviceIoControlFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,           FileHandle);
	K_INIT_ARG(XboxTypes::HANDLE,           Event);
	K_INIT_ARG(XboxTypes::PIO_APC_ROUTINE,  ApcRoutine);
	K_INIT_ARG(XboxTypes::PVOID,            ApcContext);
	K_INIT_ARG(XboxTypes::PIO_STATUS_BLOCK, IoStatusBlock);
	K_INIT_ARG(XboxTypes::ULONG,            IoControlCode);
	K_INIT_ARG(XboxTypes::PVOID,            InputBuffer);
	K_INIT_ARG(XboxTypes::ULONG,            InputBufferLength);
	K_INIT_ARG(XboxTypes::PVOID,            OutputBuffer);
	K_INIT_ARG(XboxTypes::ULONG,            OutputBufferLength);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
