#include "common.h"

/*
 * NtQueryDirectoryFile
 *
 * Import Number:      207
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE                 FileHandle
 * Parameter 1:        HANDLE                 Event
 * Parameter 2:        PIO_APC_ROUTINE        ApcRoutine
 * Parameter 3:        PVOID                  ApcContext
 * Parameter 4:        PIO_STATUS_BLOCK       IoStatusBlock
 * Parameter 5:        PVOID                  FileInformation
 * Parameter 6:        ULONG                  Length
 * Parameter 7:        FILE_INFORMATION_CLASS FileInformationClass
 * Parameter 8:        POBJECT_STRING         FileName
 * Parameter 9:        BOOLEAN                RestartScan
 * Return Type:        NTSTATUS
 */
int Xbox::NtQueryDirectoryFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(HANDLE,                 FileHandle);
	K_INIT_ARG(HANDLE,                 Event);
	K_INIT_ARG(PIO_APC_ROUTINE,        ApcRoutine);
	K_INIT_ARG(PVOID,                  ApcContext);
	K_INIT_ARG(PIO_STATUS_BLOCK,       IoStatusBlock);
	K_INIT_ARG(PVOID,                  FileInformation);
	K_INIT_ARG(ULONG,                  Length);
	K_INIT_ARG(FILE_INFORMATION_CLASS, FileInformationClass);
	K_INIT_ARG(POBJECT_STRING,         FileName);
	K_INIT_ARG(BOOLEAN,                RestartScan);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
