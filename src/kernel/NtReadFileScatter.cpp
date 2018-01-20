#include "common.h"

/*
 * NtReadFileScatter
 *
 * Import Number:      220
 * Calling Convention: stdcall
 * Parameter 0:        HANDLE                FileHandle
 * Parameter 1:        HANDLE                Event
 * Parameter 2:        PIO_APC_ROUTINE       ApcRoutine
 * Parameter 3:        PVOID                 ApcContext
 * Parameter 4:        PIO_STATUS_BLOCK      IoStatusBlock
 * Parameter 5:        PFILE_SEGMENT_ELEMENT SegmentArray
 * Parameter 6:        ULONG                 Length
 * Parameter 7:        PLARGE_INTEGER        ByteOffset
 * Return Type:        NTSTATUS
 */
int Xbox::NtReadFileScatter()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::HANDLE,                FileHandle);
	K_INIT_ARG(XboxTypes::HANDLE,                Event);
	K_INIT_ARG(XboxTypes::PIO_APC_ROUTINE,       ApcRoutine);
	K_INIT_ARG(XboxTypes::PVOID,                 ApcContext);
	K_INIT_ARG(XboxTypes::PIO_STATUS_BLOCK,      IoStatusBlock);
	K_INIT_ARG(XboxTypes::PFILE_SEGMENT_ELEMENT, SegmentArray);
	K_INIT_ARG(XboxTypes::ULONG,                 Length);
	K_INIT_ARG(XboxTypes::PLARGE_INTEGER,        ByteOffset);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
