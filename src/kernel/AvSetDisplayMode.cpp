#include "common.h"

/*
 * AvSetDisplayMode
 *
 * Import Number:      3
 * Calling Convention: stdcall
 * Parameter 0:        PVOID RegisterBase
 * Parameter 1:        ULONG Step
 * Parameter 2:        ULONG DisplayMode
 * Parameter 3:        ULONG SourceColorFormat
 * Parameter 4:        ULONG Pitch
 * Parameter 5:        ULONG FrameBuffer
 * Return Type:        ULONG
 */
int Xbox::AvSetDisplayMode()
{
	K_ENTER_STDCALL();
	K_INIT_ARG(PVOID, RegisterBase);
	K_INIT_ARG(ULONG, Step);
	K_INIT_ARG(ULONG, DisplayMode);
	K_INIT_ARG(ULONG, SourceColorFormat);
	K_INIT_ARG(ULONG, Pitch);
	K_INIT_ARG(ULONG, FrameBuffer);
	ULONG rval;

	log_debug("%s(%08x, %x, %x, %x, %x, %x)\n",
		__func__,
		RegisterBase,
		Step,
		DisplayMode,
		SourceColorFormat,
		Pitch,
		FrameBuffer);

	rval = 0;

	K_EXIT_WITH_VALUE(rval);
	return 0;
}
