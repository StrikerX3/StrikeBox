#include "common.h"

/*
 * IoCreateFile
 *
 * Import Number:      66
 * Calling Convention: stdcall
 * Parameter 0:        PHANDLE            FileHandle
 * Parameter 1:        ACCESS_MASK        DesiredAccess
 * Parameter 2:        POBJECT_ATTRIBUTES ObjectAttributes
 * Parameter 3:        PIO_STATUS_BLOCK   IoStatusBlock
 * Parameter 4:        PLARGE_INTEGER     AllocationSize
 * Parameter 5:        ULONG              FileAttributes
 * Parameter 6:        ULONG              ShareAccess
 * Parameter 7:        ULONG              Disposition
 * Parameter 8:        ULONG              CreateOptions
 * Parameter 9:        ULONG              Options
 * Return Type:        NTSTATUS
 */
int Xbox::IoCreateFile()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PHANDLE,            FileHandle);
	K_INIT_ARG(ACCESS_MASK,        DesiredAccess);
	K_INIT_ARG(POBJECT_ATTRIBUTES, ObjectAttributes);
	K_INIT_ARG(PIO_STATUS_BLOCK,   IoStatusBlock);
	K_INIT_ARG(PLARGE_INTEGER,     AllocationSize);
	K_INIT_ARG(ULONG,              FileAttributes);
	K_INIT_ARG(ULONG,              ShareAccess);
	K_INIT_ARG(ULONG,              Disposition);
	K_INIT_ARG(ULONG,              CreateOptions);
	K_INIT_ARG(ULONG,              Options);
	NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
