#include "common.h"

/*
 * IoQueryFileInformation
 *
 * Import Number:      75
 * Calling Convention: stdcall
 * Parameter 0:        PFILE_OBJECT           FileObject
 * Parameter 1:        FILE_INFORMATION_CLASS FileInformationClass
 * Parameter 2:        ULONG                  Length
 * Parameter 3:        PVOID                  FileInformation
 * Parameter 4:        PULONG                 ReturnedLength
 * Return Type:        NTSTATUS
 */
int Xbox::IoQueryFileInformation()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(XboxTypes::PFILE_OBJECT,           FileObject);
	K_INIT_ARG(XboxTypes::FILE_INFORMATION_CLASS, FileInformationClass);
	K_INIT_ARG(XboxTypes::ULONG,                  Length);
	K_INIT_ARG(XboxTypes::PVOID,                  FileInformation);
	K_INIT_ARG(XboxTypes::PULONG,                 ReturnedLength);
	XboxTypes::NTSTATUS rval;

	K_EXIT_WITH_VALUE(rval);
	return ERROR_NOT_IMPLEMENTED;
}
