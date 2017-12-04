#include "../../src/kernel/types.h"

XBAPI UCHAR XePublicKeyData[284];
XBAPI ANSI_STRING XeImageFileName[1];
XBAPI volatile OBJECT_TYPE PsThreadObjectType;
XBAPI volatile OBJECT_HANDLE_TABLE ObpObjectHandleTable;
XBAPI volatile OBJECT_TYPE ObSymbolicLinkObjectType;
XBAPI volatile OBJECT_TYPE ObDirectoryObjectType;
XBAPI volatile PMMGLOBALDATA MmGlobalData;
XBAPI volatile ULONG KiBugCheckData[];
XBAPI volatile CONST ULONG KeTimeIncrement;
XBAPI volatile DWORD KeTickCount;
XBAPI volatile KSYSTEM_TIME KeSystemTime;
XBAPI volatile KSYSTEM_TIME KeInterruptTime;
XBAPI volatile BOOLEAN KdDebuggerNotPresent;
XBAPI volatile BOOLEAN KdDebuggerEnabled;
XBAPI volatile OBJECT_TYPE IoFileObjectType;
XBAPI volatile OBJECT_TYPE IoDeviceObjectType;
XBAPI volatile OBJECT_TYPE IoCompletionObjectType;
XBAPI volatile IDE_CHANNEL_OBJECT IdexChannelObject;
XBAPI volatile STRING HalDiskSerialNumber;
XBAPI volatile STRING HalDiskModelNumber;
XBAPI volatile ULONG HalDiskCachePartitionCount;
XBAPI volatile OBJECT_TYPE ExTimerObjectType;
XBAPI volatile OBJECT_TYPE ExSemaphoreObjectType;
XBAPI volatile OBJECT_TYPE ExMutantObjectType;
XBAPI volatile OBJECT_TYPE ExEventObjectType;
XBAPI XBOX_KEY_DATA XboxSignatureKey;
XBAPI XBOX_KEY_DATA XboxLANKey;
XBAPI XBOX_KRNL_VERSION XboxKrnlVersion;
XBAPI XBOX_HARDWARE_INFO XboxHardwareInfo;
XBAPI XBOX_KEY_DATA XboxHDKey;
XBAPI XBOX_KEY_DATA XboxEEPROMKey;
XBAPI XBOX_KEY_DATA XboxAlternateSignatureKeys[];



/**
 * Decrements the reference count of the section and unloads it if the reference count reaches zero.
 * @param Section The section to be unloaded.
 * @return STATUS_SUCCESS or the error.
 */
XBAPI NTSTATUS NTAPI XeUnloadSection
(
    IN OUT PXBEIMAGE_SECTION Section
);

/**
 * Loads the supplied section into memory or increases its reference count if it's already loaded.
 * @param Section The section to be loaded.
 * @return STATUS_SUCCESS or the error.
 */
XBAPI NTSTATUS NTAPI XeLoadSection
(
    IN PXBEIMAGE_SECTION Section
);

XBAPI BOOLEAN NTAPI XcVerifyPKCS1Signature
(
    IN PUCHAR pbSig,
    IN PUCHAR pbPubKey,
    IN PUCHAR pbDigest
);

XBAPI VOID NTAPI XcUpdateCrypto
(
    IN PCRYPTO_VECTOR pNewVector,
    OUT PCRYPTO_VECTOR pROMVector OPTIONAL
);

XBAPI VOID NTAPI XcSHAUpdate
(
    IN OUT PUCHAR pbSHAContext,
    IN PUCHAR pbInput,
    IN ULONG dwInputLength
);

XBAPI VOID NTAPI XcSHAInit
(
    OUT PUCHAR pbSHAContext
);

XBAPI VOID NTAPI XcSHAFinal
(
    IN PUCHAR pbSHAContext,
    OUT PUCHAR pbDigest
);

XBAPI VOID NTAPI XcRC4Key
(
    OUT PUCHAR pbKeyStruct,
    IN ULONG dwKeyLength,
    IN PUCHAR pbKey
);

XBAPI VOID NTAPI XcRC4Crypt
(
    IN PUCHAR pbKeyStruct,
    IN ULONG dwInputLength,
    IN OUT PUCHAR pbInput
);

XBAPI ULONG NTAPI XcPKGetKeyLen
(
    IN PUCHAR pbPubKey
);

XBAPI ULONG NTAPI XcPKEncPublic
(
    IN PUCHAR pbPubKey,
    IN PUCHAR pbInput,
    OUT PUCHAR pbOutput
);

XBAPI ULONG NTAPI XcPKDecPrivate
(
    IN PUCHAR pbPrvKey,
    IN PUCHAR pbInput,
    OUT PUCHAR pbOutput
);

XBAPI ULONG NTAPI XcModExp
(
    OUT PULONG pA,
    IN PULONG pB,
    IN PULONG pC,
    IN PULONG pD,
    IN ULONG dwN
);

XBAPI VOID NTAPI XcKeyTable
(
    IN ULONG dwCipher,
    OUT PUCHAR pbKeyTable,
    IN PUCHAR pbKey
);

XBAPI VOID NTAPI XcHMAC
(
    IN PUCHAR pbKey,
    IN ULONG dwKeyLength,
    IN PUCHAR pbInput,
    IN ULONG dwInputLength,
    IN PUCHAR pbInput2,
    IN ULONG dwInputLength2,
    OUT PUCHAR pbDigest
);

XBAPI VOID NTAPI XcDESKeyParity
(
    IN OUT PUCHAR pbKey,
    IN ULONG dwKeyLength
);

XBAPI ULONG NTAPI XcCryptService
(
    IN ULONG dwOp,
    IN PVOID pArgs
);

XBAPI VOID NTAPI XcBlockCryptCBC
(
    IN ULONG dwCipher,
    IN ULONG dwInputLength,
    OUT PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp,
    IN PUCHAR pbFeedback
);

XBAPI VOID NTAPI XcBlockCrypt
(
    IN ULONG dwCipher,
    OUT PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp
);

XBAPI VOID NTAPI WRITE_PORT_BUFFER_USHORT
(
    IN PUSHORT Port,
    IN PUSHORT Buffer,
    IN ULONG Count
);

XBAPI VOID NTAPI WRITE_PORT_BUFFER_ULONG
(
    IN PULONG Port,
    IN PULONG Buffer,
    IN ULONG Count
);

XBAPI VOID NTAPI WRITE_PORT_BUFFER_UCHAR
(
    IN PUCHAR Port,
    IN PUCHAR Buffer,
    IN ULONG Count
);

/**
 * Fills a specified memory area with zeroes
 * @param Destination A pointer to the memory block which is to be filled
 * @param Length The length of the memory block which is to be filled
 */
XBAPI VOID NTAPI RtlZeroMemory
(
    IN PVOID Destination,
    IN SIZE_T Length
);

XBAPI ULONG NTAPI RtlWalkFrameChain
(
    OUT PPVOID Callers,
    IN ULONG Count,
    IN ULONG Flags
);

XBAPI VOID CDECL RtlVsprintf
(
    PCHAR pszDest,
    LPCSTR pszFormat,
    ...
);

XBAPI VOID CDECL RtlVsnprintf
(
    PCHAR pszDest,
    SIZE_T cbDest,
    LPCSTR pszFormat,
    ...
);

XBAPI VOID NTAPI RtlUpperString
(
    PSTRING DestinationString,
    PSTRING SourceString
);

/**
 * Returns the uppercased equivalent of a single character
 * @param Character The character which will be converted
 * @return The uppercased character
 */
XBAPI CHAR NTAPI RtlUpperChar
(
    CHAR Character
);

XBAPI NTSTATUS NTAPI RtlUpcaseUnicodeToMultiByteN
(
    PCHAR MultiByteString,
    ULONG MaxBytesInMultiByteString,
    PULONG BytesInMultiByteString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
);

XBAPI NTSTATUS NTAPI RtlUpcaseUnicodeString
(
    PUNICODE_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
);

XBAPI WCHAR NTAPI RtlUpcaseUnicodeChar
(
    WCHAR SourceCharacter
);

XBAPI VOID NTAPI RtlUnwind
(
    IN PVOID TargetFrame OPTIONAL,
    IN PVOID TargetIp OPTIONAL,
    IN PEXCEPTION_RECORD ExceptionRecord OPTIONAL,
    IN PVOID ReturnValue
);

XBAPI NTSTATUS NTAPI RtlUnicodeToMultiByteSize
(
    PULONG BytesInMultiByteString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
);

XBAPI NTSTATUS NTAPI RtlUnicodeToMultiByteN
(
    PCHAR MultiByteString,
    ULONG MaxBytesInMultiByteString,
    PULONG BytesInMultiByteString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
);

XBAPI NTSTATUS XBAPI RtlUnicodeStringToInteger
(
    PUNICODE_STRING String,
    ULONG Base,
    PULONG Value
);

XBAPI NTSTATUS NTAPI RtlUnicodeStringToAnsiString
(
    OUT PSTRING DestinationString,
    IN PUNICODE_STRING SourceString,
    IN BOOLEAN AllocateDestinationString
);

/**
 * Attempts to enter a critical section without blocking. If the call is successful, the calling thread takes ownership of the critical section.
 * @param CriticalSection A pointer to the critical section object.
 * @return If the critical section is successfully entered or the current thread already owns the critical section, the return value is nonzero. If another thread already owns the critical section, the return value is zero.
 */
XBAPI BOOLEAN NTAPI RtlTryEnterCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

/**
 * The RtlTimeToTimeFields routine converts system time into a TIME_FIELDS structure
 * @param Time Pointer to a buffer containing the absolute system time as a large integer, accurate to 100-nanosecond resolution
 * @param TimeFields Pointer to a caller-allocated buffer, which must be at least sizeof(TIME_FIELDS), to contain the returned information
 */
XBAPI VOID NTAPI RtlTimeToTimeFields
(
    IN PLARGE_INTEGER Time,
    OUT PTIME_FIELDS TimeFields
);

XBAPI BOOLEAN NTAPI RtlTimeFieldsToTime
(
    IN PTIME_FIELDS TimeFields,
    OUT PLARGE_INTEGER Time
);

XBAPI VOID CDECL RtlSprintf
(
    PCHAR pszDest,
    LPCSTR pszFormat,
    ...
);

XBAPI VOID CDECL RtlSnprintf
(
    PCHAR pszDest,
    SIZE_T cbDest,
    LPCSTR pszFormat,
    ...
);

XBAPI VOID NTAPI RtlRip
(
    IN PVOID ApiName,
    IN PVOID Expression,
    IN PVOID Message
);

XBAPI VOID NTAPI RtlRaiseStatus
(
    IN NTSTATUS Status
);

XBAPI VOID NTAPI RtlRaiseException
(
    IN PEXCEPTION_RECORD ExceptionRecord
);

XBAPI ULONG NTAPI RtlNtStatusToDosError
(
    IN NTSTATUS Status
);

XBAPI NTSTATUS NTAPI RtlMultiByteToUnicodeSize
(
    PULONG BytesInUnicodeString,
    PCHAR MultiByteString,
    ULONG BytesInMultiByteString
);

XBAPI NTSTATUS NTAPI RtlMultiByteToUnicodeN
(
    PWSTR UnicodeString,
    ULONG MaxBytesInUnicodeString,
    PULONG BytesInUnicodeString,
    PCHAR MultiByteString,
    ULONG BytesInMultiByteString
);

XBAPI VOID NTAPI RtlMoveMemory
(
    PVOID Destination,
    CONST PPVOID Source,
    ULONG Length
);

XBAPI VOID NTAPI RtlMapGenericMask
(
    PACCESS_MASK AccessMask,
    PGENERIC_MAPPING GenericMapping
);

/**
 * Returns the lowercase equivalent of a single character
 * @param Character The character which will be converted
 * @return The lowercased character
 */
XBAPI CHAR NTAPI RtlLowerChar
(
    IN CHAR Character
);

XBAPI VOID NTAPI RtlLeaveCriticalSectionAndRegion
(
    PRTL_CRITICAL_SECTION CriticalSection
);

/**
 * Releases ownership of the specified critical section object.
 * @param CriticalSection A pointer to the critical section object.
 */
XBAPI VOID NTAPI RtlLeaveCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

XBAPI NTSTATUS NTAPI RtlIntegerToUnicodeString
(
    ULONG Value,
    ULONG Base,
    PUNICODE_STRING String
);

XBAPI NTSTATUS NTAPI RtlIntegerToChar
(
    ULONG Value,
    ULONG Base,
    LONG OutputLength,
    PSZ String
);

/**
 * Initializes a critical section object.
 * @param CriticalSection A pointer to the critical section object.
 */
XBAPI VOID NTAPI RtlInitializeCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

XBAPI VOID NTAPI RtlInitUnicodeString
(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
);

XBAPI VOID NTAPI RtlInitAnsiString
(
    PANSI_STRING DestinationString,
    IN PCSZ SourceString
);

XBAPI VOID NTAPI RtlGetCallersAddress
(
    OUT PPVOID CallersAddress,
    OUT PPVOID CallersCaller
);

XBAPI VOID NTAPI RtlFreeUnicodeString
(
    PUNICODE_STRING UnicodeString
);

XBAPI VOID NTAPI RtlFreeAnsiString
(
    PANSI_STRING AnsiString
);

/**
 * Fills a specified memory area with repetitions of a ULONG value
 * @param Destination A pointer to the (ULONG-aligned) memory block which is to be filled
 * @param Length The length of the memory block which is to be filled
 * @param Fill The ULONG-value with which the memory block will be filled
 */
XBAPI VOID NTAPI RtlFillMemoryUlong
(
    PVOID Destination,
    SIZE_T Length,
    ULONG Pattern
);

/**
 * Fills a specified memory area with a specified value
 * @param Destination A pointer to the memory block which is to be filled
 * @param Length The length of the memory block which is to be filled
 * @param Fill The byte-value with which the memory block will be filled
 */
XBAPI VOID NTAPI RtlFillMemory
(
    PVOID Destination,
    ULONG Length,
    UCHAR Fill
);

XBAPI LARGE_INTEGER NTAPI RtlExtendedMagicDivide
(
    LARGE_INTEGER Dividend,
    LARGE_INTEGER MagicDivisor,
    CCHAR ShiftCount
);

XBAPI LARGE_INTEGER NTAPI RtlExtendedLargeIntegerDivide
(
    LARGE_INTEGER Dividend,
    ULONG Divisor,
    PULONG Remainder
);

XBAPI LARGE_INTEGER NTAPI RtlExtendedIntegerMultiply
(
    LARGE_INTEGER Multiplicand,
    LONG Multiplier
);

/**
 * Compares two counted unicode strings (UNICODE_STRING objects, NOT C-style strings!) for equality
 * @param String1 Pointer to the first unicode string
 * @param String2 Pointer to the second unicode string
 * @param CaseInSensitive Whether to ignore the case of the strings or not
 * @return TRUE if the strings are equal, FALSE if not
 */
XBAPI BOOLEAN NTAPI RtlEqualUnicodeString
(
    IN CONST PUNICODE_STRING String1,
    IN CONST PUNICODE_STRING String2,
    IN BOOLEAN CaseInSensitive
);

/**
 * Compares two counted strings (ANSI_STRING objects, NOT C-style strings!) for equality
 * @param String1 Pointer to the first string
 * @param String2 Pointer to the second string
 * @param CaseInSensitive Whether to ignore the case of the strings or not
 * @return TRUE if the strings are equal, FALSE if not
 */
XBAPI BOOLEAN NTAPI RtlEqualString
(
    IN CONST PSTRING String1,
    IN CONST PSTRING String2,
    IN BOOLEAN CaseInSensitive
);

XBAPI VOID NTAPI RtlEnterCriticalSectionAndRegion
(
    PRTL_CRITICAL_SECTION CriticalSection
);

/**
 * Waits for ownership of the specified critical section object. The function returns when the calling thread is granted ownership.
 * @param CriticalSection A pointer to the critical section object.
 */
XBAPI VOID NTAPI RtlEnterCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

XBAPI NTSTATUS NTAPI RtlDowncaseUnicodeString
(
    OUT PUNICODE_STRING DestinationString,
    IN PUNICODE_STRING SourceString,
    IN BOOLEAN AllocateDestinationString
);

XBAPI WCHAR NTAPI RtlDowncaseUnicodeChar
(
    WCHAR SourceCharacter
);

XBAPI BOOLEAN NTAPI RtlCreateUnicodeString
(
    OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
);

XBAPI VOID NTAPI RtlCopyUnicodeString
(
    PUNICODE_STRING DestinationString,
    PUNICODE_STRING SourceString
);

XBAPI VOID NTAPI RtlCopyString
(
    OUT PSTRING DestinationString,
    IN PSTRING SourceString
);

/**
 * Compares two counted unicode strings (UNICODE_STRING objects, NOT C-style strings!)
 * @param String1 Pointer to the first string
 * @param String2 Pointer to the second string
 * @param CaseInSensitive Whether to ignore the case of the strings or not
 * @return Zero if the strings are equal, less than zero if String1 is less than String2, greater than zero if String1 is greater than String2
 */
XBAPI LONG NTAPI RtlCompareUnicodeString
(
    PUNICODE_STRING String1,
    PUNICODE_STRING String2,
    BOOLEAN CaseInSensitive
);

/**
 * Compares two counted strings (ANSI_STRING objects, NOT C-style strings!)
 * @param String1 Pointer to the first string
 * @param String2 Pointer to the second string
 * @param CaseInSensitive Whether to ignore the case of the strings or not
 * @return Zero if the strings are equal, less than zero if String1 is less than String2, greater than zero if String1 is greater than String2
 */
XBAPI LONG NTAPI RtlCompareString
(
    IN CONST PSTRING String1,
    IN CONST PSTRING String2,
    IN BOOLEAN CaseInSensitive
);

/**
 * Returns how many bytes in a block of memory match a specified pattern.
 * @param Source Pointer to a block of memory. Must be aligned on a ULONG boundary.
 * @param Length Number of bytes over which the comparison should be done. Must be a multiple of sizeof(ULONG).
 * @param Pattern Pattern to be compared byte by byte, repeatedly, through the specified memory range.
 * @return The number of bytes that were compared and found to be equal. If all bytes compare as equal, the "Length"-value is returned. If "Source" is not ULONG-aligned or if "Length" is not a multiple of sizeof(ULONG), zero is returned.
 */
XBAPI SIZE_T NTAPI RtlCompareMemoryUlong
(
    PVOID Source,
    SIZE_T Length,
    ULONG Pattern
);

/**
 * Compares two blocks of memory and returns the number of bytes that match.
 * @param Source1 A pointer to the first block of memory.
 * @param Source2 A pointer to the second block of memory.
 * @param Length The number of bytes to compare.
 * @return The number of bytes in the two blocks that match. If all bytes match, the "Length"-value is returned.
 */
XBAPI SIZE_T NTAPI RtlCompareMemory
(
    IN CONST PVOID Source1,
    IN CONST PVOID Source2,
    IN SIZE_T Length
);

/**
 * Converts a single-byte character string (C-style string, NOT an ANSI_STRING object!) to an integer value
 * @param String1 Pointer to a null-terminated single-byte string
 * @param Base Specifies the base (decimal, binary, octal, hexadecimal). If not given, the routine looks for prefixes in the given string (0x, 0o, 0b), default is decimal.
 * @param Value Pointer to a ULONG variable where the converted value will be stored
 * @return STATUS_SUCCESS if the string was successfully converted, STATUS_INVALID_PARAMETER otherwise
 */
XBAPI NTSTATUS NTAPI RtlCharToInteger
(
    IN PCSZ String,
    IN ULONG Base OPTIONAL,
    OUT PULONG Value
);

XBAPI USHORT NTAPI RtlCaptureStackBackTrace
(
    IN ULONG FramesToSkip,
    IN ULONG FramesToCapture,
    OUT PPVOID BackTrace,
    OUT PULONG BackTraceHash
);

XBAPI VOID NTAPI RtlCaptureContext
(
    OUT PCONTEXT ContextRecord
);

XBAPI VOID NTAPI RtlAssert
(
    IN PVOID FailedAssertion,
    IN PVOID FileName,
    IN ULONG LineNumber,
    IN PCHAR Message OPTIONAL
);

XBAPI NTSTATUS NTAPI RtlAppendUnicodeToString
(
    PUNICODE_STRING Destination,
    PCWSTR Source
);

XBAPI NTSTATUS NTAPI RtlAppendUnicodeStringToString
(
    PUNICODE_STRING Destination,
    PUNICODE_STRING Source
);

XBAPI NTSTATUS NTAPI RtlAppendStringToString
(
    IN PSTRING Destination,
    IN PSTRING Source
);

XBAPI NTSTATUS NTAPI RtlAnsiStringToUnicodeString
(
    PUNICODE_STRING DestinationString,
    PSTRING SourceString,
    BOOLEAN AllocateDestinationString
);

XBAPI VOID NTAPI READ_PORT_BUFFER_USHORT
(
    IN PUSHORT Port,
    OUT PUSHORT Buffer,
    IN ULONG Count
);

XBAPI VOID NTAPI READ_PORT_BUFFER_ULONG
(
    IN PULONG Port,
    OUT PULONG Buffer,
    IN PULONG Count
);

XBAPI VOID NTAPI READ_PORT_BUFFER_UCHAR
(
    IN PUCHAR Port,
    OUT PUCHAR Buffer,
    IN ULONG Count
);


XBAPI VOID NTAPI __attribute__ ((noreturn)) PsTerminateSystemThread
(
    IN NTSTATUS ExitStatus
);

XBAPI NTSTATUS NTAPI PsSetCreateThreadNotifyRoutine
(
    IN PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine
);

XBAPI NTSTATUS NTAPI PsQueryStatistics
(
    IN OUT PPS_STATISTICS ProcessStatistics
);


XBAPI NTSTATUS NTAPI PsCreateSystemThreadEx
(
    OUT PHANDLE ThreadHandle,
    IN SIZE_T ThreadExtensionSize,
    IN SIZE_T KernelStackSize,
    IN SIZE_T TlsDataSize,
    OUT PHANDLE ThreadId OPTIONAL,
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext,
    IN BOOLEAN CreateSuspended,
    IN BOOLEAN DebuggerThread,
    IN PKSYSTEM_ROUTINE SystemRoutine OPTIONAL
);

XBAPI NTSTATUS NTAPI PsCreateSystemThread
(
    OUT PHANDLE ThreadHandle,
    OUT PHANDLE ThreadId OPTIONAL,
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext,
    IN BOOLEAN DebuggerThread
);

/**
 * Initialize the ethernet PHY
 * @param forceReset Whether to force a reset
 * @param param Optional parameters (seemingly unused)
 * @return Status code (zero on success)
 */
XBAPI NTSTATUS NTAPI PhyInitialize
(
    BOOLEAN forceReset,
    PVOID param OPTIONAL
);

/**
 * Read the status information from the NICs' registers
 * @param update
 * @return Flags describing the status of the NIC
 */
XBAPI DWORD NTAPI PhyGetLinkState
(
    BOOLEAN update
);


XBAPI NTSTATUS NTAPI ObReferenceObjectByPointer
(
    IN PVOID Object,
    IN POBJECT_TYPE ObjectType
);

XBAPI NTSTATUS NTAPI ObReferenceObjectByName
(
    IN POBJECT_STRING ObjectName,
    IN ULONG Attributes,
    IN POBJECT_TYPE ObjectType,
    IN OUT PVOID ParseContext OPTIONAL,
    OUT PPVOID Object
);

XBAPI BOOLEAN NTAPI ObReferenceObjectByHandle
(
    IN HANDLE Handle,
    IN POBJECT_TYPE ObjectType OPTIONAL,
    OUT PPVOID ReturnedObject
);

XBAPI NTSTATUS NTAPI ObOpenObjectByPointer
(
    IN PVOID Object,
    IN POBJECT_TYPE ObjectType,
    OUT PHANDLE Handle
);

XBAPI NTSTATUS NTAPI ObOpenObjectByName
(
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN POBJECT_TYPE ObjectType,
    IN OUT PVOID ParseContext OPTIONAL,
    OUT PHANDLE Handle
);

XBAPI VOID NTAPI ObMakeTemporaryObject
(
    IN PVOID Object
);

XBAPI NTSTATUS NTAPI ObInsertObject
(
    IN PVOID Object,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN ULONG ObjectPointerBias,
    OUT PHANDLE Handle
);


XBAPI NTSTATUS NTAPI ObCreateObject
(
    IN POBJECT_TYPE ObjectType,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN ULONG ObjectBodySize,
    OUT PPVOID Object
);

/**
 * yields execution of the current thread for one timeslice
 */
XBAPI NTSTATUS NTAPI NtYieldExecution(void);

XBAPI BOOLEAN NTAPI NtWriteFileGather
(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PFILE_SEGMENT_ELEMENT SegmentArray,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL
);

XBAPI NTSTATUS NTAPI NtWriteFile
(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL
);

/**
 * Waits until the specified object attains a state of "signaled". Also see NtWaitForSingleObject.
 * @param Handle The handle to the wait object.
 * @param WaitMode Specifies the processor mode in which the wait is to occur.
 * @param Alertable Specifies whether an alert can be delivered when the object is waiting (watch out for the STATUS_ALERTED return value).
 * @param Timeout An optional (set to NULL when not used) pointer to an absolute or relative time over which the wait is to occur. If an explicit timeout value of zero is specified, then no wait occurs if the wait cannot be satisfied immediately.
 * @return The wait completion status. Can be STATUS_SUCCESS (specified object satisifed the wait), STATUS_TIMEOUT (a timeout occured), STATUS_ALERTED (the wait was aborted to deliver an alert to the current thread) or STATUS_USER_APC (the wait was aborted to deliver a user APC to the current thread).
 */
XBAPI NTSTATUS NTAPI NtWaitForSingleObjectEx
(
    IN HANDLE Handle,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

/**
 * Waits until the specified object attains a state of "signaled". When used with a semaphore, the semaphore gets decremented and the threaad continues, or the thread waits until the semaphore-count becomes non-zero and then decrements the seamphore count and continues.
 * @param Handle The handle to the wait object.
 * @param Alertable Specifies whether an alert can be delivered when the object is waiting (watch out for the STATUS_ALERTED return value).
 * @param Timeout An optional (set to NULL when not used) pointer to an absolute or relative time over which the wait is to occur. If an explicit timeout value of zero is specified, then no wait occurs if the wait cannot be satisfied immediately.
 * @return The wait completion status. Can be STATUS_SUCCESS (specified object satisifed the wait), STATUS_TIMEOUT (a timeout occured), STATUS_ALERTED (the wait was aborted to deliver an alert to the current thread) or STATUS_USER_APC (the wait was aborted to deliver a user APC to the current thread).
 */
XBAPI NTSTATUS NTAPI NtWaitForSingleObject
(
    IN HANDLE Handle,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

/**
 * Waits until the specified objects attain a state of "signaled". The wait can be specified to either wait until all objects are signaled or until one of the objects is signaled. Also see NtWaitForSingleObject.
 * @param Count Specifies the number of objects that are to be waited on.
 * @param Handles An array of handles to wait objects.
 * @param WaitType Specifies the type of wait to perform (WaitAll or WaitAny)
 * @param WaitMode Specifies the processor mode in which the wait is to occur.
 * @param Alertable Specifies whether an alert can be delivered when the object is waiting (watch out for the STATUS_ALERTED return value).
 * @param Timeout An optional (set to NULL when not used) pointer to an absolute or relative time over which the wait is to occur. If an explicit timeout value of zero is specified, then no wait occurs if the wait cannot be satisfied immediately.
 * @return The wait completion status. The index of the object in the array that satisfied the wait is returned. Can also be STATUS_TIMEOUT (a timeout occured), STATUS_ALERTED (the wait was aborted to deliver an alert to the current thread) or STATUS_USER_APC (the wait was aborted to deliver a user APC to the current thread).
 */
XBAPI NTSTATUS NTAPI NtWaitForMultipleObjectsEx
(
    IN ULONG Count,
    IN HANDLE Handles[],
    IN WAIT_TYPE WaitType,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

XBAPI VOID NTAPI NtUserIoApcDispatcher
(
    IN PVOID ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG Reserved
);

/**
 * Suspends the target thread and optionally returns the previous suspend count.
 * @param The handle of the thread object to suspend.
 * @param PreviousSuspendCount Optional pointer to a variable that receives the thread's previous suspend count.
 * @return The status of the operation.
 */
XBAPI NTSTATUS NTAPI NtSuspendThread
(
    IN HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount OPTIONAL
);

XBAPI NTSTATUS NTAPI NtSignalAndWaitForSingleObjectEx
(
    IN HANDLE SignalHandle,
    IN HANDLE WaitHandle,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

XBAPI NTSTATUS NTAPI NtSetTimerEx
(
    IN HANDLE TimerHandle,
    IN PLARGE_INTEGER DueTime,
    IN PTIMER_APC_ROUTINE TimerApcRoutine OPTIONAL,
    IN KPROCESSOR_MODE ApcMode,
    IN PVOID TimerContext OPTIONAL,
    IN BOOLEAN ResumeTimer,
    IN LONG Period OPTIONAL,
    OUT PBOOLEAN PreviousState OPTIONAL
);

/**
 * Sets the current system time and optionally returns the old system time.
 * @param SystemTime A pointer to the new value for the system time.
 * @param PreviousTime An optional pointer to a variable that receives the previous system time.
 * @return The status of the operation. STATUS_SUCCESS when successfull, STATUS_ACCESS_VIOLATION if the input parameter cannot be read or the output cannot be written, STATUS_INVALID_PARAMETER if the input time is negative.
 */
XBAPI NTSTATUS NTAPI NtSetSystemTime
(
    IN PLARGE_INTEGER SystemTime,
    OUT PLARGE_INTEGER PreviousTime OPTIONAL
);

XBAPI NTSTATUS NTAPI NtSetIoCompletion
(
    IN HANDLE IoCompletionHandle,
    IN PVOID KeyContext,
    IN PVOID ApcContext,
    IN NTSTATUS IoStatus,
    IN ULONG_PTR IoStatusInformation
);

XBAPI NTSTATUS NTAPI NtSetInformationFile
(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
);

XBAPI NTSTATUS NTAPI NtSetEvent
(
    IN HANDLE EventHandle,
    OUT PLONG PreviousState OPTIONAL
);

/**
 * Resumes the target thread (see NtSuspendThread) and optionally returns the previous suspend count.
 * @param The handle of the thread object to resume.
 * @param PreviousSuspendCount Optional pointer to a variable that receives the thread's previous suspend count.
 * @return The status of the operation.
 */
XBAPI NTSTATUS NTAPI NtResumeThread
(
    IN HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount OPTIONAL
);

XBAPI NTSTATUS NTAPI NtRemoveIoCompletion
(
    IN HANDLE IoCompletionHandle,
    OUT PPVOID KeyContext,
    OUT PPVOID ApcContext,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER Timeout
);

/**
 * Releases a semaphore object. When the semaphore is released, the current count of the semaphore is incremented by "ReleaseCount". Any threads that are waiting for the semaphore are examined to see if the current semaphore value is sufficient to satisfy their wait. If the value specified by "ReleaseCount" would cause the maximum count for the semaphore to be exceeded, then the count for the semaphore is not affected and an error status is returned.
 * @param SemaphoreHandle An open handle to a semaphore object.
 * @param ReleaseCount The release count for the semaphore. The count must be greater than zero and less than the maximum value specified for the semaphore.
 * @param PreviousCount An optional pointer to a variable that receives the previous count for the semaphore.
 * @return The status of the release operation, STATUS_SUCCESS on success.
 */
XBAPI NTSTATUS NTAPI NtReleaseSemaphore
(
    IN HANDLE SemaphoreHandle,
    IN LONG ReleaseCount,
    OUT PLONG PreviousCount OPTIONAL
);

/**
 * Releases a mutant object (mutex).
 * @param MutantHandle The handle to the mutant object.
 * @param PreviousCount An optional pointer to a variable that receives the previous mutant count.
 * @return The status of the operation.
 */
XBAPI NTSTATUS NTAPI NtReleaseMutant
(
    IN HANDLE MutantHandle,
    OUT PLONG PreviousCount OPTIONAL
);

XBAPI NTSTATUS NTAPI NtReadFileScatter
(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PFILE_SEGMENT_ELEMENT SegmentArray,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL
);

XBAPI NTSTATUS NTAPI NtReadFile
(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL
);

XBAPI NTSTATUS NTAPI NtQueueApcThread
(
    IN HANDLE ThreadHandle,
    IN PPS_APC_ROUTINE ApcRoutine,
    IN PVOID ApcArgument1,
    IN PVOID ApcArgument2,
    IN PVOID ApcArgument3
);

XBAPI NTSTATUS NTAPI NtQueryVolumeInformationFile
(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FsInformation,
    IN ULONG Length,
    IN FS_INFORMATION_CLASS FsInformationClass
);

XBAPI NTSTATUS NTAPI NtQueryVirtualMemory
(
    IN PVOID BaseAddress,
    OUT PMEMORY_BASIC_INFORMATION MemoryInformation
);

XBAPI NTSTATUS NTAPI NtQueryTimer
(
    IN HANDLE TimerHandle,
    OUT PTIMER_BASIC_INFORMATION TimerInformation
);

XBAPI NTSTATUS NTAPI NtQuerySymbolicLinkObject
(
    IN HANDLE LinkHandle,
    IN OUT POBJECT_STRING LinkTarget,
    OUT PULONG ReturnedLength OPTIONAL
);

/**
 * Queries the state of a semaphore object.
 * @param SemaphoreHandle An open handle to a semaphore object.
 * @param SemaphoreInformation A pointer to a buffer (SEMAPHORE_BASIC_INFORMATION-structure) that receives the queried information.
 * @return The status code of the query operation, STATUS_SUCCESS on success.
 */
XBAPI NTSTATUS NTAPI NtQuerySemaphore
(
    IN HANDLE SemaphoreHandle,
    OUT PSEMAPHORE_BASIC_INFORMATION SemaphoreInformation
);

/**
 * Queries the state of a mutant object.
 * @param MutantHandle A handle to a mutant object.
 * @param MutantInformation A pointer to a MUTANT_BASIC_INFORMATION-structure that receives the requested information.
 * @return The status of the operation.
 */
XBAPI NTSTATUS NTAPI NtQueryMutant
(
    IN HANDLE MutantHandle,
    OUT PMUTANT_BASIC_INFORMATION MutantInformation
);

XBAPI NTSTATUS NTAPI NtQueryIoCompletion
(
    IN HANDLE IoCompletionHandle,
    OUT PIO_COMPLETION_BASIC_INFORMATION IoCompletionInformation
);

XBAPI NTSTATUS NTAPI NtQueryInformationFile
(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
);

XBAPI NTSTATUS NTAPI NtQueryFullAttributesFile
(
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PFILE_NETWORK_OPEN_INFORMATION FileInformation
);

XBAPI NTSTATUS NTAPI NtQueryEvent
(
    IN HANDLE EventHandle,
    OUT PEVENT_BASIC_INFORMATION EventInformation
);

XBAPI NTSTATUS NTAPI NtQueryDirectoryObject
(
    IN HANDLE DirectoryHandle,
    OUT PVOID Buffer,
    IN ULONG Length,
    IN BOOLEAN RestartScan,
    IN OUT PULONG Context,
    OUT PULONG ReturnLength OPTIONAL
);

XBAPI NTSTATUS NTAPI NtQueryDirectoryFile
(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass,
    IN POBJECT_STRING FileName OPTIONAL,
    IN BOOLEAN RestartScan
);

XBAPI NTSTATUS NTAPI NtPulseEvent
(
    IN HANDLE EventHandle,
    OUT PLONG PreviousState OPTIONAL
);

XBAPI NTSTATUS NTAPI NtProtectVirtualMemory
(
    IN OUT PPVOID BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG NewProtect,
    OUT PULONG OldProtect
);

XBAPI NTSTATUS NTAPI NtOpenSymbolicLinkObject
(
    OUT PHANDLE LinkHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes
);

XBAPI NTSTATUS NTAPI NtOpenFile
(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess,
    IN ULONG OpenOptions
);

XBAPI NTSTATUS NTAPI NtOpenDirectoryObject
(
    OUT PHANDLE DirectoryHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes
);

XBAPI NTSTATUS NTAPI NtFsControlFile
(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG FsControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength
);

XBAPI NTSTATUS NTAPI NtFreeVirtualMemory
(
    IN OUT PPVOID BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG FreeType
);

XBAPI NTSTATUS NTAPI NtFlushBuffersFile
(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock
);

XBAPI NTSTATUS NTAPI NtDuplicateObject
(
    IN HANDLE SourceHandle,
    OUT PHANDLE TargetHandle,
    IN ULONG Options
);

XBAPI NTSTATUS NTAPI NtDeviceIoControlFile
(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG IoControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength
);


XBAPI BOOLEAN NTAPI NtDeleteFile
(
    IN POBJECT_ATTRIBUTES ObjectAttributes
);

XBAPI NTSTATUS NTAPI NtCreateTimer
(
    OUT PHANDLE TimerHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN TIMER_TYPE TimerType
);

/**
 * Create a semaphore object with the specified initial and maximum count.
 * @param SemaphoreHandle A pointer to a variable that receives the value of the semaphore object handle.
 * @param ObjectAttributes An optional pointer to a structure that specifies the object's attributes.
 * @param InitialCount The initial count for the semaphore, this value must be positive and less than or equal to the maximum count.
 * @param MaximumCount The maximum count for the semaphore, this value must be greater than zero.
 * @return STATUS_SUCCESS on success or error code.
 */
XBAPI NTSTATUS NTAPI NtCreateSemaphore
(
    OUT PHANDLE SemaphoreHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN LONG InitialCount,
    IN LONG MaximumCount
);

/**
 * Creates a mutant object (mutex), sets its initial count to one (which means "signaled"), and opens a handle to the object.
 * @param MutantHandle A pointer to a variable that receives the mutant object handle.
 * @param ObjectAttributes A pointer to a OBJECT_ATTRIBUTES-structure that specifies object attributes.
 * @pararm InitialOwner A boolean value that specifies whether the creator of the mutant object wants immediate ownership.
 * @return The status of the operation.
 */
XBAPI NTSTATUS NTAPI NtCreateMutant
(
    OUT PHANDLE MutantHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN BOOLEAN InitialOwner
);

XBAPI NTSTATUS NTAPI NtCreateIoCompletion
(
    OUT PHANDLE IoCompletionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN ULONG Count OPTIONAL
);

XBAPI NTSTATUS NTAPI NtCreateFile
(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions
);


XBAPI NTSTATUS NTAPI NtCreateEvent
(
    OUT PHANDLE EventHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN EVENT_TYPE EventType,
    IN BOOLEAN InitialState
);

XBAPI NTSTATUS NTAPI NtCreateDirectoryObject
(
    OUT PHANDLE DirectoryHandle,
    IN POBJECT_ATTRIBUTES ObjectAttributes
);

/**
 * Closes an object handle.
 * @param Handle Handle to an object
 * @return STATE_SUCCESS on success, error code (e.g. STATUS_INVALID_HANDLE, STATUS_HANDLE_NOT_CLOSABLE) otherwise
 */
XBAPI NTSTATUS NTAPI NtClose
(
    IN HANDLE Handle
);

XBAPI NTSTATUS NTAPI NtClearEvent
(
    IN HANDLE EventHandle
);

XBAPI NTSTATUS NTAPI NtCancelTimer
(
    IN HANDLE TimerHandle,
    OUT PBOOLEAN CurrentState OPTIONAL
);

XBAPI NTSTATUS NTAPI NtAllocateVirtualMemory
(
    IN OUT PPVOID BaseAddress,
    IN ULONG_PTR ZeroBits,
    IN OUT PSIZE_T RegionSize,
    IN ULONG AllocationType,
    IN ULONG Protect
);


XBAPI PVOID NTAPI MmUnmapIoSpace
(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
);

XBAPI VOID NTAPI MmSetAddressProtect
(
    IN PVOID BaseAddress,
    IN ULONG NumberOfBytes,
    IN ULONG NewProtect
);

XBAPI NTSTATUS NTAPI MmQueryStatistics
(
    IN OUT PMM_STATISTICS MemoryStatistics
);

XBAPI SIZE_T NTAPI MmQueryAllocationSize
(
    IN PVOID BaseAddress
);

XBAPI ULONG NTAPI MmQueryAddressProtect
(
    IN PVOID VirtualAddress
);

/**
 * Marks a contiguous area of memory to be preserved across a quick reboot.
 * @param BaseAddress The virtual address of the memory area.
 * @param NumberOfBytes The number of bytes to be preserved.
 * @param Persist TRUE if the memory should be persistent, else FALSE.
 */
XBAPI VOID NTAPI MmPersistContiguousMemory
(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes,
    IN BOOLEAN Persist
);

XBAPI PVOID NTAPI MmMapIoSpace
(
    IN ULONG_PTR PhysicalAddress,
    IN SIZE_T NumberOfBytes,
    IN ULONG Protect
);

XBAPI VOID NTAPI MmLockUnlockPhysicalPage
(
    IN ULONG_PTR PhysicalAddress,
    IN BOOLEAN UnlockPage
);

XBAPI VOID NTAPI MmLockUnlockBufferPages
(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes,
    IN BOOLEAN UnlockPages
);

/**
 * Checks whether a page fault would occur for a read operation on a specified address.
 * @param VirtualAddress The virtual address to be checked.
 * @return TRUE if a page fault would occur, FALSE if not.
 */
XBAPI BOOLEAN NTAPI MmIsAddressValid
(
    IN PVOID VirtualAddress
);


/**
 * Returns the physical address for a virtual address.
 * @param BaseAddress A valid virtual address for which the physical address is to be returned.
 * @return The corresponding physical address.
 */
XBAPI ULONG_PTR NTAPI MmGetPhysicalAddress
(
    IN PVOID BaseAddress
);

XBAPI ULONG NTAPI MmFreeSystemMemory
(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
);

XBAPI VOID NTAPI MmFreeContiguousMemory
(
    IN PVOID BaseAddress
);

XBAPI VOID NTAPI MmDeleteKernelStack
(
    IN PVOID KernelStackBase,
    IN PVOID KernelStackLimit
);

XBAPI PVOID NTAPI MmDbgWriteCheck
(
    IN PVOID VirtualAddress,
    IN PHARDWARE_PTE Opaque
);

XBAPI VOID NTAPI MmDbgReleaseAddress
(
    IN PVOID VirtualAddress,
    IN PHARDWARE_PTE Opaque
);

XBAPI PFN_COUNT NTAPI MmDbgQueryAvailablePages (void);

XBAPI ULONG NTAPI MmDbgFreeMemory
(
    IN PVOID BaseAddress,
    IN SIZE_T NumberOfBytes
);

XBAPI PVOID NTAPI MmDbgAllocateMemory
(
    IN SIZE_T NumberOfBytes,
    IN ULONG Protect
);

XBAPI PVOID NTAPI MmCreateKernelStack
(
    IN SIZE_T NumberOfBytes,
    IN BOOLEAN DebuggerThread
);

XBAPI PVOID NTAPI MmClaimGpuInstanceMemory
(
    IN SIZE_T NumberOfBytes,
    OUT PSIZE_T NumberOfPaddingBytes
);

XBAPI PVOID NTAPI MmAllocateSystemMemory
(
    IN SIZE_T NumberOfBytes,
    IN ULONG Protect
);

XBAPI PVOID NTAPI MmAllocateContiguousMemoryEx
(
    IN SIZE_T NumberOfBytes,
    IN ULONG_PTR LowestAcceptableAddress,
    IN ULONG_PTR HighestAcceptableAddress,
    IN ULONG_PTR Alignment,
    IN ULONG Protect
);

XBAPI PVOID NTAPI MmAllocateContiguousMemory
(
    IN SIZE_T NumberOfBytes
);

XBAPI PLAUNCH_DATA_PAGE LaunchDataPage;


XBAPI NTSTATUS NTAPI KeWaitForSingleObject
(
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

XBAPI NTSTATUS NTAPI KeWaitForMultipleObjects
(
    IN ULONG Count,
    IN PVOID Object[],
    IN WAIT_TYPE WaitType,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL,
    IN PKWAIT_BLOCK WaitBlockArray
);


/**
 * Contains the number of milliseconds elapsed since the system was started.
 */

XBAPI BOOLEAN NTAPI KeTestAlertThread
(
    IN KPROCESSOR_MODE ProcessorMode
);


XBAPI BOOLEAN NTAPI KeSynchronizeExecution
(
    IN PKINTERRUPT Interrupt,
    IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,
    IN PVOID SynchronizeContext
);

XBAPI ULONG NTAPI KeSuspendThread
(
    IN PKTHREAD Thread
);

/**
 * Stalls the caller on the current processor for a specified time Interval.
 * @param MicroSeconds Specifies the number of microseconds to stall.
 */
XBAPI VOID NTAPI KeStallExecutionProcessor
(
    IN ULONG MicroSeconds
);

XBAPI BOOLEAN NTAPI KeSetTimerEx
(
    IN PKTIMER Timer,
    IN LARGE_INTEGER DueTime,
    IN LONG Period OPTIONAL,
    IN PKDPC Dpc OPTIONAL
);

XBAPI BOOLEAN NTAPI KeSetTimer
(
    IN PKTIMER Timer,
    IN LARGE_INTEGER DueTime,
    IN PKDPC Dpc OPTIONAL
);

/**
 * Sets the run-time priority of a thread.
 * @param Thread Pointer to the thread.
 * @param Priority Specifies the priority of the thread, usually to the real-time priority value, LOW_REALTIME_PRIORITY. The value LOW_PRIORITY is reserved for system use.
 * @return Returns the old priority of the thread.
 */
XBAPI KPRIORITY NTAPI KeSetPriorityThread
(
    IN PKTHREAD Thread,
    IN KPRIORITY Priority
);

XBAPI KPRIORITY NTAPI KeSetPriorityProcess
(
    IN PKPROCESS Process,
    IN KPRIORITY BasePriority
);

XBAPI VOID NTAPI KeSetEventBoostPriority
(
    IN PRKEVENT Event,
    IN PPRKTHREAD Thread OPTIONAL
);

XBAPI LONG NTAPI KeSetEvent
(
    IN PRKEVENT Event,
    IN KPRIORITY Increment,
    IN BOOLEAN Wait
);

XBAPI LOGICAL NTAPI KeSetDisableBoostThread
(
    IN PKTHREAD Thread,
    IN LOGICAL Disable
);

XBAPI LONG NTAPI KeSetBasePriorityThread
(
    IN PKTHREAD Thread,
    IN LONG Increment
);

XBAPI NTSTATUS NTAPI KeSaveFloatingPointState
(
    OUT PKFLOATING_SAVE FloatSave
);

XBAPI PLIST_ENTRY NTAPI KeRundownQueue
(
    IN PRKQUEUE Queue
);

XBAPI ULONG NTAPI KeResumeThread
(
    IN PKTHREAD Thread
);

XBAPI NTSTATUS KeRestoreFloatingPointState
(
    IN PKFLOATING_SAVE FloatSave
);

XBAPI LONG NTAPI KeResetEvent
(
    IN PRKEVENT Event
);

XBAPI BOOLEAN NTAPI KeRemoveQueueDpc
(
    IN PRKDPC Dpc
);

XBAPI PLIST_ENTRY NTAPI KeRemoveQueue
(
    IN PRKQUEUE Queue,
    IN KPROCESSOR_MODE WaitMode,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

XBAPI BOOLEAN NTAPI KeRemoveEntryDeviceQueue
(
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE DeviceQueueEntry
);

XBAPI PKDEVICE_QUEUE_ENTRY NTAPI KeRemoveDeviceQueue
(
    IN PKDEVICE_QUEUE DeviceQueue
);

XBAPI PKDEVICE_QUEUE_ENTRY NTAPI KeRemoveByKeyDeviceQueue
(
    IN PKDEVICE_QUEUE DeviceQueue,
    IN ULONG SortKey
);

XBAPI LONG NTAPI KeReleaseSemaphore
(
    IN PRKSEMAPHORE Semaphore,
    IN KPRIORITY Increment,
    IN LONG Adjustment,
    IN BOOLEAN Wait
);

XBAPI LONG NTAPI KeReleaseMutant
(
    IN PRKMUTANT Mutant,
    IN KPRIORITY Increment,
    IN BOOLEAN Abandoned,
    IN BOOLEAN Wait
);

XBAPI KIRQL NTAPI KeRaiseIrqlToSynchLevel (void);
XBAPI KIRQL NTAPI KeRaiseIrqlToDpcLevel (void);

/**
 * Obtains the current system time.
 * @param CurrentTime The system time in 100-nanosecond intervals since January 1, 1601, in GMT.
 */
XBAPI VOID NTAPI KeQuerySystemTime
(
    OUT PLARGE_INTEGER CurrentTime
);

XBAPI ULONGLONG NTAPI KeQueryPerformanceFrequency(void);

XBAPI ULONGLONG NTAPI KeQueryPerformanceCounter(void);

XBAPI ULONGLONG NTAPI KeQueryInterruptTime (void);

XBAPI LONG NTAPI KeQueryBasePriorityThread
(
    IN PKTHREAD Thread
);

XBAPI LONG NTAPI KePulseEvent
(
    IN PRKEVENT Event,
    IN KPRIORITY Increment,
    IN BOOLEAN Wait
);

XBAPI VOID NTAPI KeLeaveCriticalRegion (void);

XBAPI BOOLEAN NTAPI KeIsExecutingDpc (void);


XBAPI BOOLEAN NTAPI KeInsertQueueDpc
(
    IN PRKDPC Dpc,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
);

XBAPI BOOLEAN NTAPI KeInsertQueueApc
(
    IN PRKAPC Apc,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2,
    IN KPRIORITY Increment
);

XBAPI LONG NTAPI KeInsertQueue
(
    IN PRKQUEUE Queue,
    IN PLIST_ENTRY Entry
);

XBAPI LONG NTAPI KeInsertHeadQueue
(
    IN PRKQUEUE Queue,
    IN PLIST_ENTRY Entry
);

XBAPI BOOLEAN NTAPI KeInsertDeviceQueue
(
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
);

XBAPI BOOLEAN NTAPI KeInsertByKeyDeviceQueue
(
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry,
    IN ULONG SortKey
);

XBAPI VOID NTAPI KeInitializeTimerEx
(
    IN PKTIMER Timer,
    IN TIMER_TYPE Type
);

XBAPI VOID NTAPI KeInitializeSemaphore
(
    IN PRKSEMAPHORE Semaphore,
    IN LONG Count,
    IN LONG Limit
);

XBAPI VOID NTAPI KeInitializeQueue
(
    IN PRKQUEUE Queue,
    IN ULONG Count OPTIONAL
);

XBAPI VOID NTAPI KeInitializeMutant
(
    IN PRKMUTANT Mutant,
    IN BOOLEAN InitialOwner
);

XBAPI VOID NTAPI KeInitializeInterrupt
(
    IN PKINTERRUPT Interrupt,
    IN PKSERVICE_ROUTINE ServiceRoutine,
    IN PVOID ServiceContext,
    IN ULONG Vector,
    IN KIRQL Irql,
    IN KINTERRUPT_MODE InterruptMode,
    IN BOOLEAN ShareVector
);

XBAPI VOID NTAPI KeInitializeEvent
(
    IN PRKEVENT Event,
    IN EVENT_TYPE Type,
    IN BOOLEAN State
);

XBAPI VOID NTAPI KeInitializeDpc
(
    OUT PKDPC Dpc,
    IN PKDEFERRED_ROUTINE DeferredRoutine,
    IN PVOID DeferredContext OPTIONAL
);

XBAPI VOID NTAPI KeInitializeDeviceQueue
(
    OUT PKDEVICE_QUEUE DeviceQueue
);

XBAPI VOID NTAPI KeInitializeApc
(
    IN PRKAPC Apc,
    IN PRKTHREAD Thread,
    IN PKKERNEL_ROUTINE KernelRoutine,
    IN PKRUNDOWN_ROUTINE RundownRoutine OPTIONAL,
    IN PKNORMAL_ROUTINE NormalRoutine OPTIONAL,
    IN KPROCESSOR_MODE ProcessorMode OPTIONAL,
    IN PVOID NormalContext OPTIONAL
);

/**
 * Returns a pointer to the thread object belonging to the current thread.
 * @return A pointer to an opaque thread object.
 */
XBAPI PKTHREAD NTAPI KeGetCurrentThread(void);

XBAPI KIRQL NTAPI KeGetCurrentIrql(void);

XBAPI VOID NTAPI KeEnterCriticalRegion(void);

XBAPI BOOLEAN NTAPI KeDisconnectInterrupt
(
    IN PKINTERRUPT Interrupt
);

/**
 * Puts the current thread into an alertable or nonalertable wait state for a specified interval
 * @param WaitMode Specifies the processor mode in which the caller is waiting, which can be either KernelMode or UserMode.
 * @param Alertable TRUE if the wait is alertable.
 * @param Interval Specifies the absolute or relative time, in units of 100 nanoseconds, for which the wait is to occur. A negative value indicates relative time. Absolute expiration times track any changes in system time, relative expiration times are not affected by system time changes.
 * @return STATUS_SUCCESS (the delay completed because the specified interval elapsed), STATUS_ALERTED (the delay completed because the thread was alerted) or STATUS_USER_APC (a user-mode APC was delivered before the specified interval expired).
 */
XBAPI NTSTATUS NTAPI KeDelayExecutionThread
(
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Interval
);

XBAPI BOOLEAN NTAPI KeConnectInterrupt
(
    IN PKINTERRUPT Interrupt
);

XBAPI BOOLEAN NTAPI KeCancelTimer
(
    IN PKTIMER Timer
);

XBAPI VOID NTAPI DECLSPEC_NORETURN KeBugCheckEx
(
    IN ULONG BugCheckCode,
    IN ULONG_PTR BugCheckParameter1,
    IN ULONG_PTR BugCheckParameter2,
    IN ULONG_PTR BugCheckParameter3,
    IN ULONG_PTR BugCheckParameter4
);

XBAPI VOID NTAPI DECLSPEC_NORETURN KeBugCheck
(
    IN ULONG BugCheckCode
);

XBAPI VOID NTAPI KeBoostPriorityThread
(
    IN PKTHREAD Thread,
    IN KPRIORITY Increment
);

XBAPI BOOLEAN NTAPI KeAlertThread
(
    IN PKTHREAD Thread,
    IN KPROCESSOR_MODE ProcessorMode
);

XBAPI ULONG NTAPI KeAlertResumeThread
(
    IN PKTHREAD Thread
);


XBAPI NTSTATUS NTAPI IoSynchronousFsdRequest
(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL
);

XBAPI NTSTATUS NTAPI IoSynchronousDeviceIoControlRequest
(
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    OUT PULONG ReturnedOutputBufferLength OPTIONAL,
    IN BOOLEAN InternalDeviceIoControl
);

XBAPI VOID NTAPI IoStartPacket
(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PULONG Key OPTIONAL
);

XBAPI VOID NTAPI IoStartNextPacketByKey
(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG Key
);

XBAPI VOID NTAPI IoStartNextPacket
(
    IN PDEVICE_OBJECT DeviceObject
);

XBAPI VOID NTAPI IoSetShareAccess
(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    OUT PSHARE_ACCESS ShareAccess
);

XBAPI NTSTATUS NTAPI IoSetIoCompletion
(
    IN PVOID IoCompletion,
    IN PVOID KeyContext,
    IN PVOID ApcContext,
    IN NTSTATUS IoStatus,
    IN ULONG_PTR IoStatusInformation
);

XBAPI VOID NTAPI IoRemoveShareAccess
(
    IN PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess
);

XBAPI VOID NTAPI IoQueueThreadIrp
(
    IN PIRP Irp
);

XBAPI NTSTATUS NTAPI IoQueryVolumeInformation
(
    IN PFILE_OBJECT FileObject,
    IN FS_INFORMATION_CLASS FsInformationClass,
    IN ULONG Length,
    OUT PVOID FsInformation,
    OUT PULONG ReturnedLength
);

XBAPI NTSTATUS NTAPI IoQueryFileInformation
(
    IN PFILE_OBJECT FileObject,
    IN FILE_INFORMATION_CLASS FileInformationClass,
    IN ULONG Length,
    OUT PVOID FileInformation,
    OUT PULONG ReturnedLength
);

XBAPI VOID NTAPI IoMarkIrpMustComplete
(
    IN OUT PIRP Irp
);

XBAPI NTSTATUS NTAPI IoInvalidDeviceRequest
(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
);

XBAPI VOID NTAPI IoInitializeIrp
(
    IN OUT PIRP Irp,
    IN USHORT PacketSize,
    IN CCHAR StackSize
);

XBAPI VOID NTAPI IoFreeIrp
(
    IN PIRP Irp
);


XBAPI NTSTATUS NTAPI IoDismountVolumeByName
(
    IN POBJECT_STRING DeviceName
);

XBAPI NTSTATUS NTAPI IoDismountVolume
(
    IN PDEVICE_OBJECT DeviceObject
);


XBAPI NTSTATUS NTAPI IoDeleteSymbolicLink
(
    IN POBJECT_STRING SymbolicLinkName
);

XBAPI VOID NTAPI IoDeleteDevice
(
    IN PDEVICE_OBJECT DeviceObject
);

XBAPI NTSTATUS NTAPI IoCreateSymbolicLink
(
    IN POBJECT_STRING SymbolicLinkName,
    IN POBJECT_STRING DeviceName
);

XBAPI NTSTATUS NTAPI IoCreateFile
(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG Disposition,
    IN ULONG CreateOptions,
    IN ULONG Options
);

XBAPI NTSTATUS NTAPI IoCreateDevice
(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG DeviceExtensionSize,
    IN POBJECT_STRING DeviceName OPTIONAL,
    IN DEVICE_TYPE DeviceType,
    IN BOOLEAN Exclusive,
    OUT PPDEVICE_OBJECT DeviceObject
);


XBAPI NTSTATUS NTAPI IoCheckShareAccess
(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess,
    IN BOOLEAN Update
);

XBAPI PIRP NTAPI IoBuildSynchronousFsdRequest
(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
);

XBAPI PIRP NTAPI IoBuildDeviceIoControlRequest
(
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN BOOLEAN InternalDeviceIoControl,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
);

XBAPI PIRP NTAPI IoBuildAsynchronousFsdRequest
(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
);

XBAPI PIRP NTAPI IoAllocateIrp
(
    IN CCHAR StackSize
);


XBAPI NTSTATUS NTAPI HalWriteSMCScratchRegister
(
    IN ULONG ScratchRegister
);

XBAPI NTSTATUS NTAPI HalWriteSMBusValue
(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN WriteWordValue,
    IN ULONG DataValue
);

XBAPI VOID DECLSPEC_NORETURN NTAPI HalReturnToFirmware
(
    IN FIRMWARE_REENTRY Routine
);

XBAPI VOID NTAPI HalRegisterShutdownNotification
(
    IN PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration,
    IN BOOLEAN Register
);

XBAPI VOID NTAPI HalReadWritePCISpace
(
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN ULONG RegisterNumber,
    IN PVOID Buffer,
    IN ULONG Length,
    IN BOOLEAN WritePCISpace
);

XBAPI NTSTATUS NTAPI HalReadSMCTrayState
(
    OUT PULONG TrayState,
    OUT PULONG TrayStateChangeCount OPTIONAL
);

XBAPI NTSTATUS NTAPI HalReadSMBusValue
(
    IN UCHAR SlaveAddress,
    IN UCHAR CommandCode,
    IN BOOLEAN ReadWordValue,
    OUT PULONG DataValue
);

/**
 * Checks whether the console is in the middle of a reset or shutdown sequence.
 * @return TRUE if the console is in the middle of a reset or shutdown sequence, else FALSE.
 */
XBAPI BOOLEAN NTAPI HalIsResetOrShutdownPending(void);

/**
 * Initiates a shutdown. May return if the SMBus-lock is already owned, the shutdown will start as soon as the lock is available.
 **/
XBAPI VOID NTAPI HalInitiateShutdown(void);

XBAPI ULONG NTAPI HalGetInterruptVector
(
    IN ULONG BusInterruptLevel,
    OUT PKIRQL Irql
);

XBAPI VOID NTAPI HalEnableSystemInterrupt
(
    IN ULONG BusInterruptLevel,
    IN KINTERRUPT_MODE InterruptMode
);

/**
 * Switches the console to secure mode, where a tray eject or tray open interrupt causes the console to reboot. After the console is switched into secure mode, it cannot switch back.
 */
XBAPI VOID NTAPI HalEnableSecureTrayEject(void);

XBAPI VOID NTAPI HalDisableSystemInterrupt
(
    IN ULONG BusInterruptLevel
);

XBAPI DWORD HalBootSMCVideoMode(void);


XBAPI NTSTATUS NTAPI FscSetCacheSize
(
    IN PFN_COUNT NumberOfCachePages
);

XBAPI VOID NTAPI FscInvalidateIdleBlocks (void);

XBAPI PFN_COUNT NTAPI FscGetCacheSize (void);


XBAPI NTSTATUS NTAPI ExSaveNonVolatileSetting
(
    IN ULONG ValueIndex,
    IN ULONG Type,
    IN CONST PVOID Value,
    IN ULONG ValueLength
);

XBAPI VOID NTAPI ExReleaseReadWriteLock
(
    IN PERWLOCK ReadWriteLock
);

XBAPI NTSTATUS NTAPI ExReadWriteRefurbInfo
(
    OUT PXBOX_REFURB_INFO RefurbInfo,
    IN ULONG ValueLength,
    BOOLEAN DoWrite
);

XBAPI VOID NTAPI ExRaiseStatus
(
    IN NTSTATUS Status
);

XBAPI VOID NTAPI ExRaiseException
(
    PEXCEPTION_RECORD ExceptionRecord
);

/**
 * Returns the size of the pool block.
 * @param PoolBlock The address of the pool block.
 * @return The size of the pool block.
 */
XBAPI ULONG NTAPI ExQueryPoolBlockSize
(
    IN PVOID PoolBlock
);

XBAPI NTSTATUS NTAPI ExQueryNonVolatileSetting
(
    IN ULONG ValueIndex,
    OUT PULONG Type,
    OUT PVOID Value,
    IN ULONG ValueLength,
    OUT PULONG ResultLength
);


XBAPI LARGE_INTEGER NTAPI ExInterlockedAddLargeInteger
(
    IN PLARGE_INTEGER Addend,
    IN LARGE_INTEGER Increment
);

XBAPI VOID NTAPI ExInitializeReadWriteLock
(
    IN PERWLOCK ReadWriteLock
);

/**
 * Deallocates a block of pool memory.
 * @param P Specifies the address of the block of pool memory being deallocated.
 */
XBAPI VOID NTAPI ExFreePool
(
    IN PVOID P
);


/**
 * Allocates pool memory and returns a pointer to the allocated block.
 * @oaram NumberOfBytes The number of bytes to allocate.
 * @param Tag The pool tag to use for the allocated memory. Specify the pool tag as a character literal of up to four characters delimited by single quotation marks (for example, 'Tag1'). The string is usually specified in reverse order (for example, '1gaT'). Each ASCII character in the tag must be a value in the range 0x20 (space) to 0x126 (tilde). Each allocation code path should use a unique pool tag to help debuggers and verifiers identify the code path.
 * @return NULL if there is insufficient memory in the free pool to satisfy the request. Otherwise, the routine returns a pointer to the allocated memory.
 */
XBAPI PVOID NTAPI ExAllocatePoolWithTag
(
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
);

/**
 * Allocates pool memory and returns a pointer to the allocated block.
 * @oaram NumberOfBytes The number of bytes to allocate.
 * @return NULL if there is insufficient memory in the free pool to satisfy the request. Otherwise, the routine returns a pointer to the allocated memory.
 */
XBAPI PVOID NTAPI ExAllocatePool
(
    IN SIZE_T NumberOfBytes
);

XBAPI VOID NTAPI ExAcquireReadWriteLockShared
(
    IN PERWLOCK ReadWriteLock
);

XBAPI VOID NTAPI ExAcquireReadWriteLockExclusive
(
    IN PERWLOCK ReadWriteLock
);

XBAPI VOID NTAPI DbgUnLoadImageSymbols
(
    PSTRING FileName,
    PVOID ImageBase,
    ULONG_PTR ProcessId
);

/**
 * WARNING: This function crashes my XBox, so probably don't use.
 * Displays a prompt-string on the debugging console, then reads a line of text from the debugging console.
 * @param Prompt The string that gets output on the debugging console.
 * @param Response Specifies where to store the response string read from the debugging console.
 * @param MaximumResponseLength Maximum number of characters that fit into the response-buffer.
 * @return Number of characters stored into the response buffer, including newline.
 */
XBAPI ULONG NTAPI DbgPrompt
(
    PCH Prompt,
    PCH Response,
    ULONG MaximumResponseLength
);

/**
 * "printf"-style output function for the kernel debugger.
 * @param Format "printf"-style format string
 * @param ... Arguments matching the format string.
 * @return A status-code.
 */
XBAPI ULONG CDECL DbgPrint
(
    PCH Format,
    ...
);

XBAPI VOID NTAPI DbgLoadImageSymbols
(
    PSTRING FileName,
    PVOID ImageBase,
    ULONG_PTR ProcessId
);

XBAPI VOID NTAPI DbgBreakPointWithStatus
(
    IN ULONG Status
);

XBAPI VOID NTAPI DbgBreakPoint (void);

XBAPI VOID NTAPI AvSetSavedDataAddress
(
    IN PVOID Address
);

XBAPI ULONG NTAPI AvSetDisplayMode
(
    IN PVOID RegisterBase,
    IN ULONG Step,
    IN ULONG DisplayMode,
    IN ULONG SourceColorFormat,
    IN ULONG Pitch,
    IN ULONG FrameBuffer
);

XBAPI VOID NTAPI AvSendTVEncoderOption
(
    IN PVOID RegisterBase,
    IN ULONG Option,
    IN ULONG Param,
    OUT PULONG Result
);

XBAPI PVOID NTAPI AvGetSavedDataAddress(void);

/**
 * Performs a byte-swap (big-endian <-> little-endian) conversion of a USHORT
 * @param Source The USHORT-value which is to be swapped
 * @return The byte-swapped value
 */
XBAPI USHORT FASTCALL RtlUshortByteSwap
(
    IN USHORT Source
);

/**
 * Performs a byte-swap (big-endian <-> little-endian) conversion of a ULONG
 * @param Source The ULONG-value which is to be swapped
 * @return The byte-swapped value
 */
XBAPI ULONG FASTCALL RtlUlongByteSwap
(
    IN ULONG Source
);

XBAPI VOID FASTCALL ObfReferenceObject
(
    IN PVOID Object
);

XBAPI VOID FASTCALL ObfDereferenceObject
(
    IN PVOID Object
);

XBAPI VOID FASTCALL KiUnlockDispatcherDatabase
(
    IN KIRQL OldIrql
);

/**
 * Raises the hardware priority to the specified IRQL value, thereby masking off interrupts of equivalent or lower IRQL on the current processor.
 * @param NewIrql Specifies the new IRQL to which the hardware priority is to be raised.
 * @return The original IRQL value to be used in a subsequent call to KfLowerIrql.
 */
XBAPI KIRQL FASTCALL KfRaiseIrql
(
    IN KIRQL NewIrql
);

/**
 * Restores the IRQL on the current processor to its original value.
 * @param NewIrql Specifies the IRQL that was returned from KfRaiseIrql.
 */
XBAPI VOID FASTCALL KfLowerIrql
(
    IN KIRQL NewIrql
);

XBAPI VOID FASTCALL IofCompleteRequest
(
    IN PDEVICE_OBJECT DeviceObject
);

XBAPI NTSTATUS FASTCALL IofCallDriver
(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
);

XBAPI PSINGLE_LIST_ENTRY FASTCALL InterlockedPushEntrySList
(
    IN PSLIST_HEADER ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry
);

XBAPI PSINGLE_LIST_ENTRY FASTCALL InterlockedPopEntrySList
(
    IN PSLIST_HEADER ListHead
);

/**
 * Atomically increments a LONG-value
 * @param Addend Pointer to the LONG-variable which is to be incremented
 * @return The resulting incremented value (also stored at 'Addend')
 */
XBAPI LONG FASTCALL InterlockedIncrement
(
    IN PLONG Addend
);

XBAPI PSINGLE_LIST_ENTRY FASTCALL InterlockedFlushSList
(
    IN PSLIST_HEADER ListHead
);

XBAPI LONG FASTCALL InterlockedExchangeAdd
(
    IN OUT PLONG Addend,
    IN LONG Increment
);

XBAPI LONG FASTCALL InterlockedExchange
(
    IN OUT PLONG Target,
    IN LONG Value
);

/**
 * Atomically decrements a LONG-value
 * @param Addend Pointer to the LONG-variable which is to be decremented
 * @return The resulting decremented value (also stored at 'Addend')
 */
XBAPI LONG FASTCALL InterlockedDecrement
(
    IN PLONG Addend
);

XBAPI LONG FASTCALL InterlockedCompareExchange
(
    IN OUT PLONG Destination,
    IN LONG ExChange,
    IN LONG Comparand
);

/**
 * Requests a software interrupt.
 * @param RequestIrql The request IRQL value.
 */
XBAPI VOID FASTCALL HalRequestSoftwareInterrupt
(
    KIRQL RequestIrql
);

/**
 * Clears a possible pending software interrupt.
 * @param RequestIrql The request IRQL value.
 */
XBAPI VOID FASTCALL HalClearSoftwareInterrupt
(
    IN KIRQL RequestIrql
);

/**
 * Atomically removes an entry from the beginning of a doubly linked list (of LIST_ENTRY structs).
 * @param ListHead Pointer to the LIST_ENTRY that serves as the list's head
 * @return A pointer to the LIST_ENTRY which was removed from the list (NULL if the list was empty).
 */
XBAPI PLIST_ENTRY FASTCALL ExfInterlockedRemoveHeadList
(
    IN PLIST_ENTRY ListHead
);

/**
 * Atomically inserts an entry at the end of a doubly linked list (of LIST_ENTRY structs).
 * @param ListHead Pointer to the LIST_ENTRY that serves as the list's head
 * @param ListEntry Pointer to the LIST_ENTRY that is to be inserted
 * @return A pointer to the last list entry before the new entry was inserted (NULL if the list was empty).
 */
XBAPI PLIST_ENTRY FASTCALL ExfInterlockedInsertTailList
(
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry
);

/**
 * Atomically inserts an entry at the beginning of a doubly linked list (of LIST_ENTRY structs).
 * @param ListHead Pointer to the LIST_ENTRY that serves as the list's head
 * @param ListEntry Pointer to the LIST_ENTRY that is to be inserted
 * @return A pointer to the first list entry before the new entry was inserted (NULL if the list was empty).
 */
XBAPI PLIST_ENTRY FASTCALL ExfInterlockedInsertHeadList
(
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry
);

/**
 * Compares one integer variable to another and, if they're equal, sets the first variable to a caller-supplied value.
 * @param Destination A pointer to an integer that will be compared and possibly replaced.
 * @param Exchange A pointer to an integer that will replace the one at Destination if the comparison results in equality.
 * @param Comparand A pointer to an integer with which the value at Destination will be compared.
 * @return The inital value of the variable at Destination when the call occured.
 */
XBAPI LONGLONG FASTCALL ExInterlockedCompareExchange64
(
    IN PLONGLONG Destination,
    IN PLONGLONG Exchange,
    IN PLONGLONG Comparand
);

/**
 * Performs an interlocked addition of a ULONG increment value to a LARGE_INTEGER variable.
 * @param Addend A pointer to the LARGE_INTEGER variable that is incremented by the value of Increment.
 * @param Increment Specifies a ULONG value that is added to the variable that Addend points to.
 */
XBAPI VOID FASTCALL ExInterlockedAddLargeStatistic
(
    IN PLARGE_INTEGER Addend,
    IN ULONG Increment
);
