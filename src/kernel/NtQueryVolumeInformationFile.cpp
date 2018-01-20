#include "common.h"

/*
 * NtQueryVolumeInformationFile
 *
 * Import Number:      218
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE               FileHandle
 * Parameter 1:        PIO_STATUS_BLOCK     IoStatusBlock
 * Parameter 2:        PVOID                FsInformation
 * Parameter 3:        ULONG                Length
 * Parameter 4:        FS_INFORMATION_CLASS FsInformationClass
 * Return Type:        NTSTATUS
 */
int Xbox::NtQueryVolumeInformationFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,               FileHandle);
	K_INIT_ARG(XboxTypes::PIO_STATUS_BLOCK,     IoStatusBlock);
	K_INIT_ARG(XboxTypes::PVOID,                FsInformation);
	K_INIT_ARG(XboxTypes::ULONG,                Length);
	K_INIT_ARG(XboxTypes::FS_INFORMATION_CLASS, FsInformationClass);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
