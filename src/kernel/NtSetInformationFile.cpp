#include "common.h"

/*
 * NtSetInformationFile
 *
 * Import Number:      226
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE                 FileHandle
 * Parameter 1:        PIO_STATUS_BLOCK       IoStatusBlock
 * Parameter 2:        PVOID                  FileInformation
 * Parameter 3:        ULONG                  Length
 * Parameter 4:        FILE_INFORMATION_CLASS FileInformationClass
 * Return Type:        NTSTATUS
 */
int Xbox::NtSetInformationFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE,                 FileHandle);
	K_INIT_ARG(PIO_STATUS_BLOCK,       IoStatusBlock);
	K_INIT_ARG(PVOID,                  FileInformation);
	K_INIT_ARG(ULONG,                  Length);
	K_INIT_ARG(FILE_INFORMATION_CLASS, FileInformationClass);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
