#include "common.h"

/*
 * NtCreateFile
 *
 * Import Number:      190
 * Calling Convention: stdcall
 * Parameter 0:        PHANDLE            FileHandle
 * Parameter 1:        ACCESS_MASK        DesiredAccess
 * Parameter 2:        POBJECT_ATTRIBUTES ObjectAttributes
 * Parameter 3:        PIO_STATUS_BLOCK   IoStatusBlock
 * Parameter 4:        PLARGE_INTEGER     AllocationSize
 * Parameter 5:        ULONG              FileAttributes
 * Parameter 6:        ULONG              ShareAccess
 * Parameter 7:        ULONG              CreateDisposition
 * Parameter 8:        ULONG              CreateOptions
 * Return Type:        NTSTATUS
 */
int Xbox::NtCreateFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PHANDLE,            FileHandle);
	K_INIT_ARG(ACCESS_MASK,        DesiredAccess);
	K_INIT_ARG(POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(PIO_STATUS_BLOCK,   IoStatusBlock);
	K_INIT_ARG(PLARGE_INTEGER,     AllocationSize);
	K_INIT_ARG(ULONG,              FileAttributes);
	K_INIT_ARG(ULONG,              ShareAccess);
	K_INIT_ARG(ULONG,              CreateDisposition);
	K_INIT_ARG(ULONG,              CreateOptions);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
