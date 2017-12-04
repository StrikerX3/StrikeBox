#include "common.h"

struct KernelVariables {
	OBJECT_TYPE         ExEventObjectType;
	OBJECT_TYPE         ExMutantObjectType;
	OBJECT_TYPE         ExSemaphoreObjectType;
	OBJECT_TYPE         ExTimerObjectType;
	ULONG               HalDiskCachePartitionCount;
	STRING              HalDiskModelNumber;
	STRING              HalDiskSerialNumber;
	OBJECT_TYPE         IoCompletionObjectType;
	OBJECT_TYPE         IoDeviceObjectType;
	OBJECT_TYPE         IoFileObjectType;
	BOOLEAN             KdDebuggerEnabled;
	BOOLEAN             KdDebuggerNotPresent;
	PMMGLOBALDATA       MmGlobalData;
	KSYSTEM_TIME        KeInterruptTime;
	KSYSTEM_TIME        KeSystemTime;
	DWORD               KeTickCount;
	ULONG               KeTimeIncrement;
	ULONG               KiBugCheckData;
	PLAUNCH_DATA_PAGE   LaunchDataPage;
	OBJECT_TYPE         ObDirectoryObjectType;
	OBJECT_HANDLE_TABLE ObpObjectHandleTable;
	OBJECT_TYPE         ObSymbolicLinkObjectType;
	OBJECT_TYPE         PsThreadObjectType;
	XBOX_KEY_DATA       XboxEEPROMKey;
	XBOX_HARDWARE_INFO  XboxHardwareInfo;
	XBOX_KEY_DATA       XboxHDKey;
	XBOX_KRNL_VERSION   XboxKrnlVersion;
	XBOX_KEY_DATA       XboxSignatureKey;
	ANSI_STRING         XeImageFileName;
	XBOX_KEY_DATA       XboxLANKey;
	XBOX_KEY_DATA       XboxAlternateSignatureKeys;
	UCHAR               XePublicKeyData;
	IDE_CHANNEL_OBJECT  IdexChannelObject;
};
