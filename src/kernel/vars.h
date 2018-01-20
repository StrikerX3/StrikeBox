#include "common.h"

struct KernelVariables {
	XboxTypes::OBJECT_TYPE         ExEventObjectType;
	XboxTypes::OBJECT_TYPE         ExMutantObjectType;
	XboxTypes::OBJECT_TYPE         ExSemaphoreObjectType;
	XboxTypes::OBJECT_TYPE         ExTimerObjectType;
	XboxTypes::ULONG               HalDiskCachePartitionCount;
	XboxTypes::STRING              HalDiskModelNumber;
	XboxTypes::STRING              HalDiskSerialNumber;
	XboxTypes::OBJECT_TYPE         IoCompletionObjectType;
	XboxTypes::OBJECT_TYPE         IoDeviceObjectType;
	XboxTypes::OBJECT_TYPE         IoFileObjectType;
	XboxTypes::BOOLEAN             KdDebuggerEnabled;
	XboxTypes::BOOLEAN             KdDebuggerNotPresent;
	XboxTypes::PMMGLOBALDATA       MmGlobalData;
	XboxTypes::KSYSTEM_TIME        KeInterruptTime;
	XboxTypes::KSYSTEM_TIME        KeSystemTime;
	XboxTypes::DWORD               KeTickCount;
	XboxTypes::ULONG               KeTimeIncrement;
	XboxTypes::ULONG               KiBugCheckData;
	XboxTypes::PLAUNCH_DATA_PAGE   LaunchDataPage;
	XboxTypes::OBJECT_TYPE         ObDirectoryObjectType;
	XboxTypes::OBJECT_HANDLE_TABLE ObpObjectHandleTable;
	XboxTypes::OBJECT_TYPE         ObSymbolicLinkObjectType;
	XboxTypes::OBJECT_TYPE         PsThreadObjectType;
	XboxTypes::XBOX_KEY_DATA       XboxEEPROMKey;
	XboxTypes::XBOX_HARDWARE_INFO  XboxHardwareInfo;
	XboxTypes::XBOX_KEY_DATA       XboxHDKey;
	XboxTypes::XBOX_KRNL_VERSION   XboxKrnlVersion;
	XboxTypes::XBOX_KEY_DATA       XboxSignatureKey;
	XboxTypes::ANSI_STRING         XeImageFileName;
	XboxTypes::XBOX_KEY_DATA       XboxLANKey;
	XboxTypes::XBOX_KEY_DATA       XboxAlternateSignatureKeys;
	XboxTypes::UCHAR               XePublicKeyData;
	XboxTypes::IDE_CHANNEL_OBJECT  IdexChannelObject;
};
