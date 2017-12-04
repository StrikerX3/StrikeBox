/**
 * @file xboxkrnl.h
 * @author Stefan Schmidt
 * @brief The complete interface to the Xbox-Kernel.
 * This specific file is licensed under the CC0 1.0.
 * Look here for details: https://creativecommons.org/publicdomain/zero/1.0/
 * 
 * MB: Updated to fix pointers to DWORD type for xplatform compat. Stripped
 * function and variable defs.
 */

#pragma once
#define DUMB_POINTERS 1

/* stop clang from crying */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"

/* MSVC-compatibility for structure-packing */
#pragma ms_struct on

#if defined(__cplusplus)
extern "C"
{
#endif

#define IN
#define OUT
#define UNALIGNED
#define OPTIONAL
// #define XBAPI __declspec(dllimport)
#define XBAPI 
#define NTAPI __attribute__((annotate("stdcall")))
// #define NTAPI __attribute__((__stdcall__))
#define CDECL __attribute__((annotate("cdecl")))
// #define CDECL __attribute__((__cdecl__))
#define FASTCALL __attribute__((annotate("fastcall")))
// #define FASTCALL __attribute__((fastcall))
#define DECLSPEC_NORETURN __attribute__((noreturn))
#define RESTRICTED_POINTER __restrict__

#ifndef NULL
    #define NULL ((PVOID)0)
#endif
#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

typedef uint32_t DWORD;

#if DUMB_POINTERS
#define DEF_POINTER_TYPE(TYPE, PTYPE) typedef DWORD PTYPE
#else
#define DEF_POINTER_TYPE(TYPE, PTYPE) typedef TYPE *PTYPE
#endif

#define CONST const

#include <stdint.h>

typedef void VOID;
DEF_POINTER_TYPE(VOID, PVOID);
DEF_POINTER_TYPE(VOID, LPVOID);
DEF_POINTER_TYPE(PVOID, PPVOID);
DEF_POINTER_TYPE(PPVOID, PPPVOID);
typedef char CHAR, CCHAR, OCHAR;
DEF_POINTER_TYPE(CHAR, POCHAR);
DEF_POINTER_TYPE(CHAR, PCHAR);
DEF_POINTER_TYPE(CHAR, PCH);
DEF_POINTER_TYPE(CHAR, LPCH);
typedef signed char SCHAR;
DEF_POINTER_TYPE(SCHAR, PSCHAR);
typedef unsigned char UCHAR, BYTE;
DEF_POINTER_TYPE(UCHAR, PUCHAR);
typedef int16_t SHORT;
DEF_POINTER_TYPE(SHORT, PSHORT);
typedef uint16_t WORD, WCHAR, USHORT, CSHORT;
DEF_POINTER_TYPE(USHORT, PUSHORT);
typedef int32_t LONG, BOOLEAN, BOOL;
DEF_POINTER_TYPE(LONG, PLONG);
DEF_POINTER_TYPE(BOOLEAN, PBOOLEAN);
typedef uint32_t ULONG, SIZE_T;
DEF_POINTER_TYPE(ULONG, PULONG);
DEF_POINTER_TYPE(SIZE_T, PSIZE_T);
typedef int64_t LONGLONG;
DEF_POINTER_TYPE(LONGLONG, PLONGLONG);
typedef uint64_t ULONGLONG;

typedef LONG NTSTATUS;
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define STATUS_INVALID_PARAMETER ((NTSTATUS)0xC000000DL)
#define STATUS_INVALID_HANDLE ((NTSTATUS)0xC0000008L)
#define STATUS_HANDLE_NOT_CLOSABLE ((NTSTATUS)0xC0000235L)

#define STATUS_WAIT_0 ((NTSTATUS)0x00000000L)
#define STATUS_ABANDONED_WAIT_0 ((NTSTATUS)0x00000080L)
#define STATUS_USER_APC ((NTSTATUS)0x000000C0L)
#define STATUS_ALERTED ((NTSTATUS)0x00000101L)
#define STATUS_TIMEOUT ((NTSTATUS)0x00000102L)
#define STATUS_PENDING ((NTSTATUS)0x00000103L)
#define STATUS_SEGMENT_NOTIFICATION ((NTSTATUS)0x40000005L)
#define STATUS_GUARD_PAGE_VIOLATION ((NTSTATUS)0x80000001L)
#define STATUS_DATATYPE_MISALIGNMENT ((NTSTATUS)0x80000002L)
#define STATUS_BREAKPOINT ((NTSTATUS)0x80000003L)
#define STATUS_SINGLE_STEP ((NTSTATUS)0x80000004L)
#define STATUS_ACCESS_VIOLATION ((NTSTATUS)0xC0000005L)
#define STATUS_IN_PAGE_ERROR ((NTSTATUS)0xC0000006L)
#define STATUS_NO_MEMORY ((NTSTATUS)0xC0000017L)
#define STATUS_ILLEGAL_INSTRUCTION ((NTSTATUS)0xC000001DL)
#define STATUS_NONCONTINUABLE_EXCEPTION ((NTSTATUS)0xC0000025L)
#define STATUS_INVALID_DISPOSITION ((NTSTATUS)0xC0000026L)
#define STATUS_OBJECT_NAME_INVALID ((NTSTATUS)0xC0000033L)
#define STATUS_OBJECT_NAME_NOT_FOUND ((NTSTATUS)0xC0000034L)
#define STATUS_OBJECT_NAME_COLLISION ((NTSTATUS)0xC0000035L)
#define STATUS_ARRAY_BOUNDS_EXCEEDED ((NTSTATUS)0xC000008CL)
#define STATUS_FLOAT_DENORMAL_OPERAND ((NTSTATUS)0xC000008DL)
#define STATUS_FLOAT_DIVIDE_BY_ZERO ((NTSTATUS)0xC000008EL)
#define STATUS_FLOAT_INEXACT_RESULT ((NTSTATUS)0xC000008FL)
#define STATUS_FLOAT_INVALID_OPERATION ((NTSTATUS)0xC0000090L)
#define STATUS_FLOAT_OVERFLOW ((NTSTATUS)0xC0000091L)
#define STATUS_FLOAT_STACK_CHECK ((NTSTATUS)0xC0000092L)
#define STATUS_FLOAT_UNDERFLOW ((NTSTATUS)0xC0000093L)
#define STATUS_INTEGER_DIVIDE_BY_ZERO ((NTSTATUS)0xC0000094L)
#define STATUS_INTEGER_OVERFLOW ((NTSTATUS)0xC0000095L)
#define STATUS_PRIVILEGED_INSTRUCTION ((NTSTATUS)0xC0000096L)
#define STATUS_DATA_OVERRUN ((NTSTATUS)0xC000003CL)
#define STATUS_INVALID_IMAGE_FORMAT ((NTSTATUS)0xC000007BL)
#define STATUS_INSUFFICIENT_RESOURCES ((NTSTATUS)0xC000009AL)
#define STATUS_STACK_OVERFLOW ((NTSTATUS)0xC00000FDL)
#define STATUS_CONTROL_C_EXIT ((NTSTATUS)0xC000013AL)
#define STATUS_TOO_MANY_SECRETS ((NTSTATUS)0xC0000156L)
#define STATUS_IMAGE_GAME_REGION_VIOLATION ((NTSTATUS)0xC0050001L)
#define STATUS_IMAGE_MEDIA_TYPE_VIOLATION ((NTSTATUS)0xC0050002L)

// Used in AvSendTVEncoderOption
#define AV_PACK_NONE 0x00000000
#define AV_PACK_STANDARD 0x00000001
#define AV_PACK_RFU 0x00000002
#define AV_PACK_SCART 0x00000003
#define AV_PACK_HDTV 0x00000004
#define AV_PACK_VGA 0x00000005
#define AV_PACK_SVIDEO 0x00000006

typedef PVOID HANDLE;
DEF_POINTER_TYPE(HANDLE, PHANDLE);
typedef ULONG PHYSICAL_ADDRESS;
DEF_POINTER_TYPE(PHYSICAL_ADDRESS, PPHYSICAL_ADDRESS);
typedef UCHAR KIRQL;
DEF_POINTER_TYPE(KIRQL, PKIRQL);
typedef ULONG ULONG_PTR;
typedef LONG LONG_PTR;
typedef ULONG PFN_COUNT;
DEF_POINTER_TYPE(PFN_COUNT, PPFN_COUNT);
typedef ULONG PFN_NUMBER;
DEF_POINTER_TYPE(PFN_NUMBER, PPFN_NUMBER);
typedef LONG KPRIORITY;
typedef ULONG DEVICE_TYPE;
typedef ULONG LOGICAL;

DEF_POINTER_TYPE(WCHAR, PWSTR);
DEF_POINTER_TYPE(DWORD, PDWORD);
DEF_POINTER_TYPE(DWORD, LPDWORD);
DEF_POINTER_TYPE(const char, PCSZ);
DEF_POINTER_TYPE(const char, PCSTR);
DEF_POINTER_TYPE(const char, LPCSTR);
DEF_POINTER_TYPE(CHAR, PSZ);
DEF_POINTER_TYPE(CHAR, PSTR);
DEF_POINTER_TYPE(CONST WCHAR, LPCWSTR);
DEF_POINTER_TYPE(CONST WCHAR, PCWSTR);

typedef ULONG ACCESS_MASK;
DEF_POINTER_TYPE(ACCESS_MASK, PACCESS_MASK);

typedef CCHAR KPROCESSOR_MODE;
typedef enum _MODE {
    KernelMode,
    UserMode,
    MaximumMode
} MODE;

/**
 * This struct defines a counted string used for ANSI-strings
 */
typedef struct _ANSI_STRING
{
    USHORT Length;        /**< Length in bytes of the string stored in the buffer */
    USHORT MaximumLength; /**< Length in bytes of the buffer */
    PSTR Buffer;          /**< Pointer to the buffer used for the character-string */
} ANSI_STRING, STRING, OBJECT_STRING;

DEF_POINTER_TYPE(struct _ANSI_STRING, PANSI_STRING);
DEF_POINTER_TYPE(struct _ANSI_STRING, PSTRING);
DEF_POINTER_TYPE(struct _ANSI_STRING, POBJECT_STRING);

/**
 * This struct defines a counted string used for UNICODE-strings
 */
typedef struct _UNICODE_STRING
{
    USHORT Length;        /**< Length in bytes of the string stored in the buffer */
    USHORT MaximumLength; /**< Length in bytes of the buffer */
    PWSTR Buffer;         /**< Pointer to the buffer used for the character-string */
} UNICODE_STRING;

DEF_POINTER_TYPE(CONST UNICODE_STRING, PCUNICODE_STRING);
DEF_POINTER_TYPE(UNICODE_STRING, PUNICODE_STRING);

/**
 * Header or descriptor for an entry in a doubly linked list.
 * Initialized by InitializeListHead, members shouldn't be updated manually.
 */
struct _LIST_ENTRY;
DEF_POINTER_TYPE(struct _LIST_ENTRY, PLIST_ENTRY);

typedef struct _LIST_ENTRY
{
	PLIST_ENTRY Flink; /**< Points to the next entry of the list or the header if there is no next entry */
	PLIST_ENTRY Blink; /**< Points to the previous entry of the list or the header if there is no previous entry */
} LIST_ENTRY;


/*
    VOID InitializeListHead (
        PLIST_ENTRY ListHead
    );
*/
#define InitializeListHead (ListHead) ((Listhead)->Flink = (ListHead)->Blink = (ListHead))

#define IsListEmpty (ListHead) ((ListHead)->Flink == (ListHead))

#define RemoveHeadList (Listhead) (ListHead)->Flink;{RemoveEntryList((ListHead)->Flink)}

#define RemoveTailList (ListHead) (ListHead)->Blink;{RemoveEntryList((ListHead)->Blink)}

#define RemoveEntryList (Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
}

#define InsertTailList (ListHead, Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
}

#define InsertHeadList (ListHead, Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
}

#define PopEntryList (ListHead) \
    (ListHead)->Next;\
    {\
        PSINGLE_LIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {\
            (ListHead)->Next = FirstEntry->Next;\
        }\
    }

#define PushEntryList (ListHead, Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry);

/**
 * Struct for modelling critical sections in the XBOX-kernel
 */
typedef struct _RTL_CRITICAL_SECTION
{
    union {
        struct {
            UCHAR Type;
            UCHAR Absolute;
            UCHAR Size;
            UCHAR Inserted;
            LONG SignalState;
            LIST_ENTRY WaitListHead;
        } Event;
        ULONG RawEvent[4];
    } Synchronization;

    LONG LockCount;
    LONG RecursionCount;
    PVOID OwningThread;
} RTL_CRITICAL_SECTION;

DEF_POINTER_TYPE(RTL_CRITICAL_SECTION, PRTL_CRITICAL_SECTION);

/**
 * MS's way to represent a 64-bit signed int on platforms that may not support
 * them directly.
 */
typedef union _LARGE_INTEGER
{
    struct
    {
        ULONG LowPart; /**< The low-order 32 bits. */
        LONG HighPart; /**< The high-order 32 bits. */
    };
    struct
    {
        ULONG LowPart; /**< The low-order 32 bits. */
        LONG HighPart; /**< The high-order 32 bits. */
    } u;
    LONGLONG QuadPart; /**< A signed 64-bit integer. */
} LARGE_INTEGER;

DEF_POINTER_TYPE(LARGE_INTEGER, PLARGE_INTEGER);

/**
 * MS's way to represent a 64-bit unsigned int on platforms that may not support
 * them directly.
 */
typedef union _ULARGE_INTEGER
{
	struct
	{
		ULONG LowPart; /**< The low-order 32 bits. */
		ULONG HighPart; /**< The high-order 32 bits. */ /**< The high-order 32 bits. */
	};
	struct
	{
		ULONG LowPart; /**< The low-order 32 bits. */
		ULONG HighPart; /**< The high-order 32 bits. */
	} u;
	ULONGLONG QuadPart; /**< An unsigned 64-bit integer. */
} ULARGE_INTEGER;

DEF_POINTER_TYPE(ULARGE_INTEGER, PULARGE_INTEGER);

/**
 * Time information
 */
typedef struct _TIME_FIELDS
{
    SHORT Year; /**< Specifies a value from 1601 on */
    SHORT Month; /**< Specifies a value from 1 to 12 */
    SHORT Day; /**< Specifies a value from 1 to 31 */
    SHORT Hour; /**< Specifies a value from 0 to 23 */
    SHORT Minute; /**< Specifies a value from 0 to 59 */
    SHORT Second; /**< Specifies a value from 0 to 59 */
    SHORT Millisecond; /**< Specifies a value from 0 to 999 */
    SHORT Weekday; /**< Specifies a value from 0 to 6 (Sunday to Saturday) */
} TIME_FIELDS;

DEF_POINTER_TYPE(TIME_FIELDS, PTIME_FIELDS);

typedef struct _OVERLAPPED
{
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    DWORD Offset;
    DWORD OffsetHigh;
    HANDLE hEvent;
} OVERLAPPED;

DEF_POINTER_TYPE(OVERLAPPED, LPOVERLAPPED);

typedef enum _WAIT_TYPE
{
    WaitAll = 0,
    WaitAny = 1
} WAIT_TYPE;

typedef struct _IO_STATUS_BLOCK
{
    union
    {
        NTSTATUS Status;
        PVOID Pointer;
    };

    ULONG_PTR Information;
} IO_STATUS_BLOCK;

DEF_POINTER_TYPE(IO_STATUS_BLOCK, PIO_STATUS_BLOCK);

typedef enum _FSINFOCLASS
{
    FileFsVolumeInformation = 1,
    FileFsLabelInformation,
    FileFsSizeInformation,
    FileFsDeviceInformation,
    FileFsAttributeInformation,
    FileFsControlInformation,
    FileFsFullSizeInformation,
    FileFsObjectIdInformation,
    FileFsMaximumInformation
} FS_INFORMATION_CLASS;

DEF_POINTER_TYPE(FS_INFORMATION_CLASS, PFS_INFORMATION_CLASS);

typedef struct _FILE_FS_LABEL_INFORMATION
{
    ULONG VolumeLabelLength;
    OCHAR VolumeLabel[1];
} FILE_FS_LABEL_INFORMATION;

DEF_POINTER_TYPE(FILE_FS_LABEL_INFORMATION, PFILE_FS_LABEL_INFORMATION);

typedef struct _FILE_FS_VOLUME_INFORMATION
{
    LARGE_INTEGER VolumeCreationTime;
    ULONG VolumeSerialNumber;
    ULONG VolumeLabelLength;
    BOOLEAN SupportsObjects;
    OCHAR VolumeLabel[1];
} FILE_FS_VOLUME_INFORMATION;

DEF_POINTER_TYPE(FILE_FS_VOLUME_INFORMATION, PFILE_FS_VOLUME_INFORMATION);

typedef struct _FILE_FS_SIZE_INFORMATION
{
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER AvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_SIZE_INFORMATION;

DEF_POINTER_TYPE(FILE_FS_SIZE_INFORMATION, PFILE_FS_SIZE_INFORMATION);

typedef struct _FILE_FS_FULL_SIZE_INFORMATION
{
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER CallerAvailableAllocationUnits;
    LARGE_INTEGER ActualAvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_FULL_SIZE_INFORMATION;

DEF_POINTER_TYPE(FILE_FS_FULL_SIZE_INFORMATION, PFILE_FS_FULL_SIZE_INFORMATION);

typedef struct _FILE_FS_OBJECTID_INFORMATION
{
    UCHAR ObjectId[16];
    UCHAR ExtendedInfo[48];
} FILE_FS_OBJECTID_INFORMATION;

DEF_POINTER_TYPE(FILE_FS_OBJECTID_INFORMATION, PFILE_FS_OBJECTID_INFORMATION);

typedef struct _FILE_FS_DEVICE_INFORMATION
{
    DEVICE_TYPE DeviceType;
    ULONG Characteristics;
} FILE_FS_DEVICE_INFORMATION;

DEF_POINTER_TYPE(FILE_FS_DEVICE_INFORMATION, PFILE_FS_DEVICE_INFORMATION);

typedef struct _FILE_FS_ATTRIBUTE_INFORMATION
{
    ULONG FileSystemAttributes;
    LONG MaximumComponentNameLength;
    ULONG FileSystemNameLength;
    OCHAR FileSystemName[1];
} FILE_FS_ATTRIBUTE_INFORMATION;

DEF_POINTER_TYPE(FILE_FS_ATTRIBUTE_INFORMATION, PFILE_FS_ATTRIBUTE_INFORMATION);

typedef struct _MEMORY_BASIC_INFORMATION
{
    PVOID BaseAddress;
    PVOID AllocationBase;
    DWORD AllocationProtect;
    DWORD RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} MEMORY_BASIC_INFORMATION;

DEF_POINTER_TYPE(MEMORY_BASIC_INFORMATION, PMEMORY_BASIC_INFORMATION);

typedef struct _SID_IDENTIFIER_AUTHORITY
{
    BYTE Value[6];
} SID_IDENTIFIER_AUTHORITY;

DEF_POINTER_TYPE(SID_IDENTIFIER_AUTHORITY, PSID_IDENTIFIER_AUTHORITY);

typedef struct _SID
{
    BYTE Revision;
    BYTE SubAuthorityCount;
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
    DWORD SubAuthority[1]; /**< Actually has size 'SubAuthorityCount' **/
} SID;

DEF_POINTER_TYPE(SID, PISID);

typedef enum _FILE_INFORMATION_CLASS
{
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileObjectIdInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileQuotaInformation,
    FileReparsePointInformation,
    FileNetworkOpenInformation,
    FileAttributeTagInformation,
    FileTrackingInformation,
    FileMaximumInformation
} FILE_INFORMATION_CLASS;

DEF_POINTER_TYPE(FILE_INFORMATION_CLASS, PFILE_INFORMATION_CLASS);

typedef struct _FILE_DIRECTORY_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    OCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION;

DEF_POINTER_TYPE(FILE_DIRECTORY_INFORMATION, PFILE_DIRECTORY_INFORMATION);

typedef struct _FILE_FULL_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    WCHAR FileName[1];
} FILE_FULL_DIR_INFORMATION;

DEF_POINTER_TYPE(FILE_FULL_DIR_INFORMATION, PFILE_FULL_DIR_INFORMATION);

typedef struct _FILE_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION;

DEF_POINTER_TYPE(FILE_BOTH_DIR_INFORMATION, PFILE_BOTH_DIR_INFORMATION);

typedef struct _FILE_BASIC_INFORMATION
{
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    ULONG FileAttributes;
} FILE_BASIC_INFORMATION;

DEF_POINTER_TYPE(FILE_BASIC_INFORMATION, PFILE_BASIC_INFORMATION);

typedef struct _FILE_STANDARD_INFORMATION
{
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG NumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
} FILE_STANDARD_INFORMATION;

DEF_POINTER_TYPE(FILE_STANDARD_INFORMATION, PFILE_STANDARD_INFORMATION);

typedef struct _FILE_INTERNAL_INFORMATION
{
    LARGE_INTEGER IndexNumber;
} FILE_INTERNAL_INFORMATION;

DEF_POINTER_TYPE(FILE_INTERNAL_INFORMATION, PFILE_INTERNAL_INFORMATION);

typedef struct _FILE_EA_INFORMATION
{
    ULONG EaSize;
} FILE_EA_INFORMATION;

DEF_POINTER_TYPE(FILE_EA_INFORMATION, PFILE_EA_INFORMATION);

typedef struct _FILE_ACCESS_INFORMATION
{
    ACCESS_MASK AccessFlags;
} FILE_ACCESS_INFORMATION;

DEF_POINTER_TYPE(FILE_ACCESS_INFORMATION, PFILE_ACCESS_INFORMATION);

typedef struct _FILE_NAME_INFORMATION
{
    ULONG FileNameLength;
    OCHAR FileName[1];
} FILE_NAME_INFORMATION;

DEF_POINTER_TYPE(FILE_NAME_INFORMATION, PFILE_NAME_INFORMATION);

typedef struct _FILE_RENAME_INFORMATION
{
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    OBJECT_STRING FileName;
} FILE_RENAME_INFORMATION;

DEF_POINTER_TYPE(FILE_RENAME_INFORMATION, PFILE_RENAME_INFORMATION);

typedef struct _FILE_LINK_INFORMATION
{
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    OCHAR FileName[1];
} FILE_LINK_INFORMATION;

DEF_POINTER_TYPE(FILE_LINK_INFORMATION, PFILE_LINK_INFORMATION);

typedef struct _FILE_NAMES_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    OCHAR FileName[1];
} FILE_NAMES_INFORMATION;

DEF_POINTER_TYPE(FILE_NAMES_INFORMATION, PFILE_NAMES_INFORMATION);

typedef struct _FILE_DISPOSITION_INFORMATION
{
    BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFORMATION;

DEF_POINTER_TYPE(FILE_DISPOSITION_INFORMATION, PFILE_DISPOSITION_INFORMATION);

typedef struct _FILE_POSITION_INFORMATION
{
    LARGE_INTEGER CurrentByteOffset;
} FILE_POSITION_INFORMATION;

DEF_POINTER_TYPE(FILE_POSITION_INFORMATION, PFILE_POSITION_INFORMATION);

typedef struct _FILE_FULL_EA_INFORMATION
{
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[1];
} FILE_FULL_EA_INFORMATION;

DEF_POINTER_TYPE(FILE_FULL_EA_INFORMATION, PFILE_FULL_EA_INFORMATION);

typedef struct _FILE_MODE_INFORMATION
{
    ULONG Mode;
} FILE_MODE_INFORMATION;

DEF_POINTER_TYPE(FILE_MODE_INFORMATION, PFILE_MODE_INFORMATION);

typedef struct _FILE_ALIGNMENT_INFORMATION
{
    ULONG AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION;

DEF_POINTER_TYPE(FILE_ALIGNMENT_INFORMATION, PFILE_ALIGNMENT_INFORMATION);

typedef struct _FILE_ALL_INFORMATION
{
    FILE_BASIC_INFORMATION BasicInformation;
    FILE_STANDARD_INFORMATION StandardInformation;
    FILE_INTERNAL_INFORMATION InternalInformation;
    FILE_EA_INFORMATION EaInformation;
    FILE_ACCESS_INFORMATION AccessInformation;
    FILE_POSITION_INFORMATION PositionInformation;
    FILE_MODE_INFORMATION ModeInformation;
    FILE_ALIGNMENT_INFORMATION AlignmentInformation;
    FILE_NAME_INFORMATION NameInformation;
} FILE_ALL_INFORMATION;

DEF_POINTER_TYPE(FILE_ALL_INFORMATION, PFILE_ALL_INFORMATION);

typedef struct _FILE_ALLOCATION_INFORMATION
{
    LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFORMATION;

DEF_POINTER_TYPE(FILE_ALLOCATION_INFORMATION, PFILE_ALLOCATION_INFORMATION);

typedef struct _FILE_END_OF_FILE_INFORMATION
{
    LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFORMATION;

DEF_POINTER_TYPE(FILE_END_OF_FILE_INFORMATION, PFILE_END_OF_FILE_INFORMATION);

typedef struct _FILE_STREAM_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG StreamNameLength;
    LARGE_INTEGER StreamSize;
    LARGE_INTEGER StreamAllocationSize;
    OCHAR StreamName[1];
} FILE_STREAM_INFORMATION;

DEF_POINTER_TYPE(FILE_STREAM_INFORMATION, PFILE_STREAM_INFORMATION);

typedef struct _FILE_PIPE_INFORMATION
{
    ULONG ReadMode;
    ULONG CompletionMode;
} FILE_PIPE_INFORMATION;

DEF_POINTER_TYPE(FILE_PIPE_INFORMATION, PFILE_PIPE_INFORMATION);

typedef struct _FILE_PIPE_LOCAL_INFORMATION
{
    ULONG NamedPipeType;
    ULONG NamedPipeConfiguration;
    ULONG MaximumInstances;
    ULONG CurrentInstances;
    ULONG InboundQuota;
    ULONG ReadDataAvailable;
    ULONG OutboundQuota;
    ULONG WriteQuotaAvailable;
    ULONG NamedPipeState;
    ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION;

DEF_POINTER_TYPE(FILE_PIPE_LOCAL_INFORMATION, PFILE_PIPE_LOCAL_INFORMATION);

typedef struct _FILE_PIPE_REMOTE_INFORMATION
{
    LARGE_INTEGER CollectDataTime;
    ULONG MaximumCollectionCount;
} FILE_PIPE_REMOTE_INFORMATION;

DEF_POINTER_TYPE(FILE_PIPE_REMOTE_INFORMATION, PFILE_PIPE_REMOTE_INFORMATION);

typedef struct _FILE_MAILSLOT_QUERY_INFORMATION
{
    ULONG MaximumMessageSize;
    ULONG MailslotQuota;
    ULONG NextMessageSize;
    ULONG MessagesAvailable;
    LARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_QUERY_INFORMATION;

DEF_POINTER_TYPE(FILE_MAILSLOT_QUERY_INFORMATION, PFILE_MAILSLOT_QUERY_INFORMATION);

typedef struct _FILE_MAILSLOT_SET_INFORMATION
{
    PLARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_SET_INFORMATION;

DEF_POINTER_TYPE(FILE_MAILSLOT_SET_INFORMATION, PFILE_MAILSLOT_SET_INFORMATION);

typedef struct _FILE_COMPRESSION_INFORMATION
{
    LARGE_INTEGER CompressedFileSize;
    USHORT CompressionFormat;
    UCHAR CompressionUnitShift;
    UCHAR ChunkShift;
    UCHAR ClusterShift;
    UCHAR Reserved[3];
} FILE_COMPRESSION_INFORMATION;

DEF_POINTER_TYPE(FILE_COMPRESSION_INFORMATION, PFILE_COMPRESSION_INFORMATION);

typedef struct _FILE_OBJECTID_INFORMATION
{
    LONGLONG FileReference;
    UCHAR ObjectId[16];
    union
    {
        struct
        {
            UCHAR BirthVolumeId[16];
            UCHAR BirthObjectId[16];
            UCHAR DomainId[16];
        };
        UCHAR ExtendedInfo[48];
    };
} FILE_OBJECTID_INFORMATION;

DEF_POINTER_TYPE(FILE_OBJECTID_INFORMATION, PFILE_OBJECTID_INFORMATION);

typedef struct _FILE_COMPLETION_INFORMATION
{
    HANDLE Port;
    PVOID Key;
} FILE_COMPLETION_INFORMATION;

DEF_POINTER_TYPE(FILE_COMPLETION_INFORMATION, PFILE_COMPLETION_INFORMATION);

typedef struct _FILE_MOVE_CLUSTER_INFORMATION
{
    ULONG ClusterCount;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    OCHAR FileName[1];
} FILE_MOVE_CLUSTER_INFORMATION;

DEF_POINTER_TYPE(FILE_MOVE_CLUSTER_INFORMATION, PFILE_MOVE_CLUSTER_INFORMATION);

typedef struct _FILE_QUOTA_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG SidLength;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER QuotaUsed;
    LARGE_INTEGER QuotaThreshold;
    LARGE_INTEGER QuotaLimit;
    SID Sid;
} FILE_QUOTA_INFORMATION;

DEF_POINTER_TYPE(FILE_QUOTA_INFORMATION, PFILE_QUOTA_INFORMATION);

typedef struct _FILE_REPARSE_POINT_INFORMATION
{
    LONGLONG FileReference;
    ULONG Tag;
} FILE_REPARSE_POINT_INFORMATION;

DEF_POINTER_TYPE(FILE_REPARSE_POINT_INFORMATION, PFILE_REPARSE_POINT_INFORMATION);

typedef struct _FILE_NETWORK_OPEN_INFORMATION
{
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION;

DEF_POINTER_TYPE(FILE_NETWORK_OPEN_INFORMATION, PFILE_NETWORK_OPEN_INFORMATION);

typedef struct _FILE_ATTRIBUTE_TAG_INFORMATION
{
    ULONG FileAttributes;
    ULONG ReparseTag;
} FILE_ATTRIBUTE_TAG_INFORMATION;

DEF_POINTER_TYPE(FILE_ATTRIBUTE_TAG_INFORMATION, PFILE_ATTRIBUTE_TAG_INFORMATION);

typedef struct _FILE_TRACKING_INFORMATION
{
    HANDLE DestinationFile;
    ULONG ObjectInformationLength;
    CHAR ObjectInformation[1];
} FILE_TRACKING_INFORMATION;

DEF_POINTER_TYPE(FILE_TRACKING_INFORMATION, PFILE_TRACKING_INFORMATION);

/**
 * Object Attributes Structure
 */
typedef struct _OBJECT_ATTRIBUTES
{
        HANDLE RootDirectory; /**< Optional handle to the root object directory for the path name specified by the ObjectName member. If RootDirectory is NULL, ObjectName must point to a fully qualified object name that includes the full path to the target object. If RootDirectory is non-NULL, ObjectName specifies an object name relative to the RootDirectory directory. */
        PANSI_STRING ObjectName; /**< Pointer to a Unicode string that contains the name of the object for which a handle is to be opened. This must either be a fully qualified object name, or a relative path name to the directory specified by the RootDirectory member. */
        ULONG Attributes; /**< Bitmask of flags that specify object handle attributes. */
} OBJECT_ATTRIBUTES;

DEF_POINTER_TYPE(OBJECT_ATTRIBUTES, POBJECT_ATTRIBUTES);

#define OBJ_INHERIT 0x00000002L
#define OBJ_PERMANENT 0x00000010L
#define OBJ_EXCLUSIVE 0x00000020L
#define OBJ_CASE_INSENSITIVE 0x00000040L
#define OBJ_OPENIF 0x00000080L
#define OBJ_OPENLINK 0x00000100L
#define OBJ_VALID_ATTRIBUTES 0x000001F2L

/*
    VOID InitializeObjectAttributes (
        OUT POBJECT_ATTRIBUTES p,
        IN PUNICODE_STRING n,
        IN ULONG a,
        IN HANDLE r,
        IN PSECURITY_DESCRIPTOR s
    );
*/
#define InitializeObjectAttributes(p, n, a, r, s) { \
    (p)->RootDirectory = r; \
    (p)->Attributes = a; \
    (p)->ObjectName = n; \
}

typedef enum _EVENT_TYPE
{
	NotificationEvent = 0,
	SynchronizationEvent
} EVENT_TYPE;

/**
 * Memory manager statistics
 */
typedef struct _MM_STATISTICS
{
	ULONG Length; /**< Length of MM_STATISTICS in bytes */
	ULONG TotalPhysicalPages;
	ULONG AvailablePages;
	ULONG VirtualMemoryBytesCommitted;
	ULONG VirtualMemoryBytesReserved;
	ULONG CachePagesCommitted;
	ULONG PoolPagesCommitted;
	ULONG StackPagesCommitted;
	ULONG ImagePagesCommitted;
} MM_STATISTICS;

DEF_POINTER_TYPE(MM_STATISTICS, PMM_STATISTICS);

typedef struct _LAUNCH_DATA_HEADER
{
	DWORD dwLaunchDataType;
	DWORD dwTitleId;
	CHAR szLaunchPath[520];
	DWORD dwFlags;
} LAUNCH_DATA_HEADER;

DEF_POINTER_TYPE(LAUNCH_DATA_HEADER, PLAUNCH_DATA_HEADER);

typedef struct _LAUNCH_DATA_PAGE
{
	LAUNCH_DATA_HEADER Header;
	UCHAR Pad[492];
	UCHAR LaunchData[3072];
} LAUNCH_DATA_PAGE;

DEF_POINTER_TYPE(LAUNCH_DATA_PAGE, PLAUNCH_DATA_PAGE);

#define LDT_LAUNCH_DASHBOARD 1
#define LDT_NONE 0xFFFFFFFF

typedef struct _DISPATCHER_HEADER
{
	UCHAR Type;
	UCHAR Absolute;
	UCHAR Size;
	UCHAR Inserted;
	LONG SignalState;
	LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER;

typedef struct _KDPC
{
	CSHORT Type;
	BOOLEAN Inserted;
	UCHAR Padding;
	LIST_ENTRY DpcListEntry;
	PVOID DeferredRoutine;
	PVOID DeferredContext;
	PVOID SystemArgument1;
	PVOID SystemArgument2;
} KDPC;

DEF_POINTER_TYPE(KDPC, PKDPC);
DEF_POINTER_TYPE(KDPC, PRKDPC);

/**
 * The kernels way of representing a timer object. MS recommends not to
 * manipulate the members directly.
 */
typedef struct _KTIMER
{
	DISPATCHER_HEADER Header;
	ULARGE_INTEGER DueTime;
	LIST_ENTRY TimerListEntry;
	PKDPC Dpc;
	LONG Period;
} KTIMER;

DEF_POINTER_TYPE(KTIMER, PKTIMER);

typedef struct _KPROCESS
{
    LIST_ENTRY ReadListHead;
    LIST_ENTRY ThreadListHead;
    ULONG StackCount;
    LONG ThreadQuantum;
    SCHAR BasePriority;
    UCHAR DisableBoost;
    UCHAR DisableQuantum;
} KPROCESS;

DEF_POINTER_TYPE(KPROCESS, PKPROCESS);

typedef struct _KAPC_STATE
{
    LIST_ENTRY ApcListHead[2];
    PKPROCESS Process;
    UCHAR KernelApcInProgress;
    UCHAR KernelApcPending;
    UCHAR UserApcPending;
    UCHAR ApcQueueable;
} KAPC_STATE;

typedef struct _KQUEUE
{
    DISPATCHER_HEADER Header;
    LIST_ENTRY EntryListHead;
    ULONG CurrentCount;
    ULONG MaximumCount;
    LIST_ENTRY ThreadListHead;
} KQUEUE;

DEF_POINTER_TYPE(KQUEUE, PKQUEUE);
DEF_POINTER_TYPE(KQUEUE, PRKQUEUE);

struct _KTHREAD;
DEF_POINTER_TYPE(struct _KTHREAD, PKTHREAD);

struct _KWAIT_BLOCK;
DEF_POINTER_TYPE(struct _KWAIT_BLOCK, PKWAIT_BLOCK);

typedef struct _KWAIT_BLOCK
{
    LIST_ENTRY WaitListEntry;
    PKTHREAD Thread;
    PVOID Object;
    PKWAIT_BLOCK NextWaitBlock;
    SHORT WaitKey;
    SHORT WaitType;
} KWAIT_BLOCK;

typedef struct _KAPC
{
    SHORT Type;
    CHAR ApcMode;
    UCHAR Inserted;
    PKTHREAD Thread;
    LIST_ENTRY ApcListEntry;
    PVOID KernelRoutine;
    PVOID RundownRoutine;
    PVOID NormalRoutine;
    PVOID NormalContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
} KAPC;

DEF_POINTER_TYPE(KAPC, PKAPC);
DEF_POINTER_TYPE(KAPC, PRKAPC);

typedef struct _KSEMAPHORE
{
    DISPATCHER_HEADER Header;
    LONG Limit;
} KSEMAPHORE;

DEF_POINTER_TYPE(KSEMAPHORE, PKSEMAPHORE);
DEF_POINTER_TYPE(KSEMAPHORE, PRKSEMAPHORE);

typedef struct _KTHREAD
{
	DISPATCHER_HEADER Header;
    LIST_ENTRY MutantListHead;
    ULONG KernelTime;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID KernelStack;
    PVOID TlsData;
    UCHAR State;
    UCHAR Alerted[2];
    UCHAR Alertable;
    UCHAR NpxState;
    CHAR Saturation;
    SCHAR Priority;
    UCHAR Padding;
    KAPC_STATE ApcState;
    ULONG ContextSwitches;
    LONG WaitStatus;
    UCHAR WaitIrql;
    CHAR WaitMode;
    UCHAR WaitNext;
    UCHAR WaitReason;
    PKWAIT_BLOCK WaitBlockList;
    LIST_ENTRY WaitListEntry;
    ULONG WaitTime;
    ULONG KernelApcDisable;
    LONG Quantum;
    SCHAR BasePriority;
    UCHAR DecrementCount;
    SCHAR PriorityDecrement;
    UCHAR DisableBoost;
    UCHAR NpxIrql;
    CHAR SuspendCount;
    UCHAR Preempted;
    UCHAR HasTerminated;
    PKQUEUE Queue;
    LIST_ENTRY QueueListEntry;
    KTIMER Timer;
    KWAIT_BLOCK TimerWaitBlock;
    KAPC SuspendApc;
    KSEMAPHORE SuspendSemaphore;
    LIST_ENTRY ThreadListEntry;
} KTHREAD;
DEF_POINTER_TYPE(KTHREAD, PKTHREAD);
DEF_POINTER_TYPE(KTHREAD, PRKTHREAD);
DEF_POINTER_TYPE(PKTHREAD, PPRKTHREAD);

typedef enum _TIMER_TYPE
{
	NotificationTimer,
	SynchronizationTimer
} TIMER_TYPE;

typedef struct _KINTERRUPT
{
    PVOID ServiceRoutine;
    PVOID ServiceContext;
    ULONG BusInterruptLevel;
	ULONG Irql;
    UCHAR Connected;
    UCHAR ShareVector;
    UCHAR Mode;
    UCHAR Padding7;
    ULONG ServiceCount;
    ULONG DispatchCode[22];
} KINTERRUPT;

DEF_POINTER_TYPE(KINTERRUPT, PKINTERRUPT);

typedef struct _KSYSTEM_TIME
{
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME;

typedef enum _FIRMWARE_REENTRY
{
	HalHaltRoutine,
	HalRebootRoutine,
	HalQuickRebootRoutine,
	HalKdRebootRoutine,
	HalFatalErrorRebootRoutine,
	HalMaximumRoutine
} FIRMWARE_REENTRY;

typedef struct _XBEIMAGE_SECTION
{
    ULONG SectionFlags;
    ULONG virtualAddress;
    ULONG VirtuaSize;
    ULONG PointerToRawData;
    ULONG SizeOfRawData;
    PUCHAR SectionName;
    ULONG SectionReferenceCount;
    PSHORT HeadSharedPageReferenceCount;
    PSHORT TailSharedPageReferenceCount;
    UCHAR SectionDigest[20];
} XBEIMAGE_SECTION;
DEF_POINTER_TYPE(XBEIMAGE_SECTION, PXBEIMAGE_SECTION);

typedef struct _OBJECT_TYPE
{
    PVOID AllocateProcedure;
    PVOID FreeProcedure;
    PVOID CloseProcedure;
    PVOID DeleteProcedure;
    PVOID ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE;
DEF_POINTER_TYPE(OBJECT_TYPE, POBJECT_TYPE);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PKDEFERRED_ROUTINE);
#else
typedef NTAPI VOID (*PKDEFERRED_ROUTINE) (
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
);
#endif

typedef struct _KDEVICE_QUEUE
{
    SHORT Type;
    UCHAR Size;
    UCHAR Busy;
    LIST_ENTRY DeviceListHead;
} KDEVICE_QUEUE;

DEF_POINTER_TYPE(KDEVICE_QUEUE, PKDEVICE_QUEUE);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PKSTART_ROUTINE);
#else
typedef VOID (*PKSTART_ROUTINE) (
    IN PVOID StartContext
);
#endif

typedef union _FILE_SEGMENT_ELEMENT
{
    PVOID Buffer;
    DWORD Alignment;
} FILE_SEGMENT_ELEMENT;

DEF_POINTER_TYPE(FILE_SEGMENT_ELEMENT, PFILE_SEGMENT_ELEMENT);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PIO_APC_ROUTINE);
#else
typedef VOID (*PIO_APC_ROUTINE) (
    IN PVOID ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG Reserved
);
#endif

typedef struct _SEMAPHORE_BASIC_INFORMATION
{
    LONG CurrentCount;
    LONG MaximumCount;
} SEMAPHORE_BASIC_INFORMATION;
DEF_POINTER_TYPE(SEMAPHORE_BASIC_INFORMATION, PSEMAPHORE_BASIC_INFORMATION);

typedef struct _MUTANT_BASIC_INFORMATION
{
    LONG CurrentCount;
    UCHAR OwnedByCaller;
    UCHAR AbandonedState;
} MUTANT_BASIC_INFORMATION;
DEF_POINTER_TYPE(MUTANT_BASIC_INFORMATION, PMUTANT_BASIC_INFORMATION);

typedef struct _IO_COMPLETION_BASIC_INFORMATION
{
    LONG Depth;
} IO_COMPLETION_BASIC_INFORMATION;
DEF_POINTER_TYPE(IO_COMPLETION_BASIC_INFORMATION, PIO_COMPLETION_BASIC_INFORMATION);

typedef struct _EVENT_BASIC_INFORMATION
{
    EVENT_TYPE EventType;
    LONG EventState;
} EVENT_BASIC_INFORMATION;
DEF_POINTER_TYPE(EVENT_BASIC_INFORMATION, PEVENT_BASIC_INFORMATION);

typedef struct _PS_STATISTICS
{
    ULONG Length;
    ULONG ThreadCount;
    ULONG HandleCount;
} PS_STATISTICS;
DEF_POINTER_TYPE(PS_STATISTICS, PPS_STATISTICS);

typedef struct _ETHREAD
{
    KTHREAD Tcb;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER ExitTime;

    union
    {
        NTSTATUS ExitStatus;
        PVOID OfsChain;
    };

    union
    {
        LIST_ENTRY ReaperListEntry;
        LIST_ENTRY ActiveTimerListHead;
    };

    HANDLE UniqueThread;
    PVOID StartAddress;
    LIST_ENTRY IrpList;
    PVOID DebugData;
} ETHREAD;
DEF_POINTER_TYPE(ETHREAD, PETHREAD);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PCREATE_THREAD_NOTIFY_ROUTINE);
#else
typedef VOID (*PCREATE_THREAD_NOTIFY_ROUTINE) (
    IN PETHREAD Thread,
    IN HANDLE ThreadId,
    IN BOOLEAN Create
);
#endif

/**
 * Enumeration type for indicating whether an interrupt is level- or
 * edge-triggered.
 */
typedef enum _KINTERRUPT_MODE
{
    LevelSensitive, /**< Interrupt is level-triggered. Used for traditional PCI line-based interrupts. */
    Latched /**< Interrupt is edge-triggered. Used for PCI message-signaled interrupts */
} KINTERRUPT_MODE;

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PKSERVICE_ROUTINE);
#else
typedef BOOLEAN (* NTAPI PKSERVICE_ROUTINE) (
    IN PKINTERRUPT Interrupt,
    IN PVOID ServiceContext
);
#endif

typedef struct _TIMER_BASIC_INFORMATION
{
    LARGE_INTEGER RemainingTime;
    BOOLEAN TimerState;
} TIMER_BASIC_INFORMATION;

DEF_POINTER_TYPE(TIMER_BASIC_INFORMATION, PTIMER_BASIC_INFORMATION);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PTIMER_APC_ROUTINE);
#else
typedef VOID (*PTIMER_APC_ROUTINE) (
    IN PVOID TimerContext,
    IN ULONG TimerLowValue,
    IN LONG TimerHighValue
);
#endif

typedef struct _XBOX_KRNL_VERSION
{
    USHORT Major;
    USHORT Minor;
    USHORT Build;
    USHORT Qfe;
} XBOX_KRNL_VERSION;

DEF_POINTER_TYPE(XBOX_KRNL_VERSION, PXBOX_KRNL_VERSION);

/**
 * Information about the XBOX-hardware
 */
typedef struct _XBOX_HARDWARE_INFO
{
    ULONG Flags;
    UCHAR GpuRevision;
    UCHAR McpRevision;
    UCHAR reserved[2];
} XBOX_HARDWARE_INFO;

#define XBOX_HW_FLAG_INTERNAL_USB_HUB 0x00000001
#define XBOX_HW_FLAG_DEVKIT_KERNEL 0x00000002
#define XBOX_480P_MACROVISION_ENABLED 0x00000004
#define XBOX_HW_FLAG_ARCADE 0x00000008

#define XBOX_KEY_LENGTH 16
typedef UCHAR XBOX_KEY_DATA[XBOX_KEY_LENGTH];

typedef struct _GENERIC_MAPPING
{
    ACCESS_MASK GenericRead;
    ACCESS_MASK GenericWrite;
    ACCESS_MASK GenericExecute;
    ACCESS_MASK GenericAll;
} GENERIC_MAPPING;

DEF_POINTER_TYPE(GENERIC_MAPPING, PGENERIC_MAPPING);

/**
 * Describes an entry in (or the header of) a singly linked list
 */
struct _SINGLE_LIST_ENTRY;
DEF_POINTER_TYPE(struct _SINGLE_LIST_ENTRY, PSINGLE_LIST_ENTRY);

struct _SINGLE_LIST_ENTRY
{
    PSINGLE_LIST_ENTRY Next; /**< Pointer to the next (or first, if this is a header) entry in the singly linked list (NULL if there is none) */
};

typedef struct _SINGLE_LIST_ENTRY SINGLE_LIST_ENTRY;

/**
 * Serves as a header for a singly linked list. Initialized by ExInitializeSListHead
 */
typedef struct _SLIST_HEADER
{
    ULONGLONG Alignment;
    struct
    {
        SINGLE_LIST_ENTRY Next;
        USHORT Depth;
        USHORT Sequence;
    };
} SLIST_HEADER;

DEF_POINTER_TYPE(SLIST_HEADER, PSLIST_HEADER);

typedef struct _KEVENT
{
    DISPATCHER_HEADER Header;
} KEVENT;

DEF_POINTER_TYPE(KEVENT, PKEVENT);
DEF_POINTER_TYPE(KEVENT, PRKEVENT);

typedef struct _KDEVICE_QUEUE_ENTRY
{
    LIST_ENTRY DeviceListEntry;
    ULONG SortKey;
    BOOLEAN Inserted;
} KDEVICE_QUEUE_ENTRY;

DEF_POINTER_TYPE(KDEVICE_QUEUE_ENTRY, PKDEVICE_QUEUE_ENTRY);

typedef struct _IO_COMPLETION_CONTEXT
{
    PVOID Port;
    PVOID Key;
} IO_COMPLETION_CONTEXT;

DEF_POINTER_TYPE(IO_COMPLETION_CONTEXT, PIO_COMPLETION_CONTEXT);

struct _DEVICE_OBJECT;
DEF_POINTER_TYPE(struct _DEVICE_OBJECT, PDEVICE_OBJECT);
DEF_POINTER_TYPE(PDEVICE_OBJECT, PPDEVICE_OBJECT);

struct _FILE_OBJECT;
DEF_POINTER_TYPE(struct _FILE_OBJECT, PFILE_OBJECT);

typedef struct _FILE_OBJECT
{
    CSHORT Type;
    BOOLEAN DeletePending : 1;
    BOOLEAN ReadAccess : 1;
    BOOLEAN WriteAccess : 1;
    BOOLEAN DeleteAccess : 1;
    BOOLEAN SharedRead : 1;
    BOOLEAN SharedWrite : 1;
    BOOLEAN SharedDelete : 1;
    BOOLEAN Reserved : 1;
    UCHAR Flags;
    PDEVICE_OBJECT DeviceObject;
    PVOID FsContext;
    PVOID FsContext2;
    NTSTATUS FinalStatus;
    LARGE_INTEGER CurrentByteOffset;
    PFILE_OBJECT RelatedFileObject;
    PIO_COMPLETION_CONTEXT CompletionContext;
    LONG LockCount;
    KEVENT Lock;
    KEVENT Event;
} FILE_OBJECT;

/**
 * This struct represents an I/O request packet
 */
typedef struct _IRP
{
    CSHORT Type;
    USHORT Size;
    ULONG Flags; /**< Flags for the packet */
    LIST_ENTRY ThreadListEntry;
    IO_STATUS_BLOCK IoStatus;
    CHAR StackCount;
    CHAR CurrentLocation;
    BOOLEAN PendingReturned;
    BOOLEAN Cancel; /**< Has the packet been canceled? */
    PIO_STATUS_BLOCK UserIosb;
    PKEVENT UserEvent;

    union
    {
        struct
        {
            PIO_APC_ROUTINE UserApcRoutine;
            PVOID UserApcContext;
        } AsynchronousParameters;
        LARGE_INTEGER AllocationSize;
    } Overlay;

    PVOID UserBuffer;
    PFILE_SEGMENT_ELEMENT SegmentArray;
    ULONG LockedBufferLength;

    union
    {
        struct
        {
            union
            {
                KDEVICE_QUEUE_ENTRY DeviceQueueEntry;
                struct
                {
                    PVOID DriverContext[5];
                };
            };
            PETHREAD Thread;

            struct
            {
                LIST_ENTRY ListEntry;
                union
                {
                    PVOID CurrentStackLocation;
                    ULONG PacketType;
                };
            };

            PFILE_OBJECT OriginalFileObject;
        } Overlay;

        KAPC Apc;
        PVOID CompletionKey;
    } Tail;
} IRP;
DEF_POINTER_TYPE(IRP, PIRP);

#define IRP_NOCACHE 0x00000001
#define IRP_MOUNT_COMPLETION 0x00000002
#define IRP_SYNCHRONOUS_API 0x00000004
#define IRP_CREATE_OPERATION 0x00000008
#define IRP_READ_OPERATION 0x00000010
#define IRP_WRITE_OPERATION 0x00000020
#define IRP_CLOSE_OPERATION 0x00000040
#define IRP_DEFER_IO_COMPLETION 0x00000080
#define IRP_OB_QUERY_NAME 0x00000100
#define IRP_UNLOCK_USER_BUFFER 0x00000200
#define IRP_SCATTER_GATHER_OPERATION 0x00000400
#define IRP_UNMAP_SEGMENT_ARRAY 0x00000800
#define IRP_NO_CANCELIO 0x00001000

struct _DEVICE_OBJECT;

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PDRIVER_STARTIO);
#else
typedef VOID (*PDRIVER_STARTIO) (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP *Irp
);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PDRIVER_DELETEDEVICE);
#else
typedef VOID (*PDRIVER_DELETEDEVICE) (
    IN PDEVICE_OBJECT DeviceObject
);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PDRIVER_DISMOUNTVOLUME);
#else
typedef NTSTATUS (*PDRIVER_DISMOUNTVOLUME) (
    IN PDEVICE_OBJECT DeviceObject
);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PDRIVER_DISPATCH);
#else
typedef NTSTATUS (*PDRIVER_DISPATCH) (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
);
#endif

typedef struct _DRIVER_OBJECT
{
    PDRIVER_STARTIO DriverStartIo;
    PDRIVER_DELETEDEVICE DriverDeleteDevice;
    PDRIVER_DISMOUNTVOLUME DriverDismountVolume;
    PDRIVER_DISPATCH MajorFunction[0x0E];
} DRIVER_OBJECT;

DEF_POINTER_TYPE(DRIVER_OBJECT, PDRIVER_OBJECT);

typedef struct _DEVICE_OBJECT
{
    CSHORT Type;
    USHORT Size;
    LONG ReferenceCount;
    PDRIVER_OBJECT DriverObject;
    PDEVICE_OBJECT MountedOrSelfDevice;
    PIRP CurrentIrp;
    ULONG Flags;
    PVOID DeviceExtension;
    UCHAR DeviceType;
    UCHAR StartIoFlags;
    CCHAR StackSize;
    BOOLEAN DeletePending;
    ULONG SectorSize;
    ULONG AlignmentRequirement;
    KDEVICE_QUEUE DeviceQueue;
    KEVENT DeviceLock;
    ULONG StartIoKey;
} DEVICE_OBJECT;

DEF_POINTER_TYPE(DEVICE_OBJECT, PDEVICE_OBJECT);

typedef struct _ERWLOCK
{
    LONG LockCount;
    ULONG WritersWaitingCount;
    ULONG ReadersWaitingCount;
    ULONG ReadersEntryCount;
    KEVENT WriterEvent;
    KSEMAPHORE ReaderSemaphore;
} ERWLOCK;

DEF_POINTER_TYPE(ERWLOCK, PERWLOCK);

struct _EXCEPTION_RECORD;
DEF_POINTER_TYPE(struct _EXCEPTION_RECORD, PEXCEPTION_RECORD);

typedef struct _EXCEPTION_RECORD
{
    NTSTATUS ExceptionCode;
    ULONG ExceptionFlags;
    PEXCEPTION_RECORD ExceptionRecord;
    PVOID ExceptionAddress;
    ULONG NumberParameters;
    ULONG_PTR ExceptionInformation[15];
} EXCEPTION_RECORD;

typedef struct _XBOX_REFURB_INFO
{
    ULONG Signature;
    ULONG PowerCycleCount;
    LARGE_INTEGER FirstSetTime;
} XBOX_REFURB_INFO;

DEF_POINTER_TYPE(XBOX_REFURB_INFO, PXBOX_REFURB_INFO);

typedef struct _FLOATING_SAVE_AREA
{
    WORD ControlWord;
    WORD StatusWord;
    WORD TagWord;
    WORD ErrorOpcode;
    DWORD ErrorOffset;
    DWORD ErrorSelector;
    DWORD DataOffset;
    DWORD DataSelector;
    DWORD MXCsr;
    DWORD Reserved2;
    BYTE RegisterArea[128];
    BYTE XmmRegisterArea[128];
    BYTE Reserved4[224];
    DWORD Cr0NpxState;
} __attribute__((packed)) FLOATING_SAVE_AREA;

DEF_POINTER_TYPE(FLOATING_SAVE_AREA, PFLOATING_SAVE_AREA);

typedef struct _CONTEXT
{
    DWORD ContextFlags;
    FLOATING_SAVE_AREA FloatSave;
    DWORD Edi;
    DWORD Esi;
    DWORD Ebx;
    DWORD Edx;
    DWORD Ecx;
    DWORD Eax;
    DWORD Ebp;
    DWORD Eip;
    DWORD SegCs;
    DWORD EFlags;
    DWORD Esp;
    DWORD SegSs;
} CONTEXT;

DEF_POINTER_TYPE(CONTEXT, PCONTEXT);

typedef struct _KFLOATING_SAVE
{
    ULONG ControlWord;
    ULONG StatusWord;
    ULONG ErrorOffset;
    ULONG ErrorSelector;
    ULONG DataOffset;
    ULONG DataSelector;
    ULONG Cr0NpxState;
    ULONG Spare1;
} KFLOATING_SAVE;

DEF_POINTER_TYPE(KFLOATING_SAVE, PKFLOATING_SAVE);

typedef struct _HARDWARE_PTE
{
    ULONG Valid : 1;
    ULONG Write : 1;
    ULONG Owner : 1;
    ULONG WriteThrough : 1;
    ULONG CacheDisable : 1;
    ULONG Accessed : 1;
    ULONG Dirty : 1;
    ULONG LargePage : 1;
    ULONG Global : 1;
    ULONG GuardOrEndOfAllocation : 1;
    ULONG PersistAllocation : 1;
    ULONG reserved : 1;
    ULONG PageFrameNumber : 20;
} HARDWARE_PTE;

DEF_POINTER_TYPE(HARDWARE_PTE, PHARDWARE_PTE);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PPS_APC_ROUTINE);
#else
typedef VOID (*PPS_APC_ROUTINE) (
    IN PVOID ApcArgument1,
    IN PVOID ApcArgument2,
    IN PVOID ApcArgument3
);
#endif

typedef enum _KWAIT_REASON
{
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPair,
    WrQueue,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    WrRendezvous,
    WrFsCacheIn,
    WrFsCacheOut,
    Spare4,
    Spare5,
    Spare6,
    WrKernel,
    MaximumWaitReason
} KWAIT_REASON;

typedef struct _KMUTANT
{
    DISPATCHER_HEADER Header;
    LIST_ENTRY MutantListEntry;
    PRKTHREAD OwnerThread;
    BOOLEAN Abandoned;
} KMUTANT;

DEF_POINTER_TYPE(KMUTANT, PKMUTANT);
DEF_POINTER_TYPE(KMUTANT, PRKMUTANT);

typedef struct _SHARE_ACCESS
{
    UCHAR OpenCount;
    UCHAR Readers;
    UCHAR Writers;
    UCHAR Deleters;
    UCHAR SharedRead;
    UCHAR SharedWrite;
    UCHAR SharedDelete;
} SHARE_ACCESS;

DEF_POINTER_TYPE(SHARE_ACCESS, PSHARE_ACCESS);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PKSYNCHRONIZE_ROUTINE);
#else
typedef BOOLEAN (*PKSYNCHRONIZE_ROUTINE) (
    IN PVOID SynchronizeContext
);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PKRUNDOWN_ROUTINE);
#else
typedef VOID (*PKRUNDOWN_ROUTINE) (
    IN PKAPC Apc
);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PKNORMAL_ROUTINE);
#else
typedef VOID (*PKNORMAL_ROUTINE) (
    IN PVOID NormalContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PKKERNEL_ROUTINE);
#else
typedef VOID (*PKKERNEL_ROUTINE) (
    IN PKAPC Apc,
    IN OUT PKNORMAL_ROUTINE *NormalRoutine,
    IN OUT PPVOID NormalContext,
    IN OUT PPVOID SystemArgument1,
    IN OUT PPVOID SystemArgument2
);
#endif

struct _HAL_SHUTDOWN_REGISTRATION;

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PHAL_SHUTDOWN_NOTIFICATION);
#else
typedef VOID (*PHAL_SHUTDOWN_NOTIFICATION) (
    IN struct _HAL_SHUTDOWN_REGISTRATION *ShutdownRegistration
);
#endif

typedef struct _HAL_SHUTDOWN_REGISTRATION
{
    PHAL_SHUTDOWN_NOTIFICATION NotificationRoutine;
    LONG Priority;
    LIST_ENTRY ListEntry;
} HAL_SHUTDOWN_REGISTRATION;

DEF_POINTER_TYPE(HAL_SHUTDOWN_REGISTRATION, PHAL_SHUTDOWN_REGISTRATION);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcSHAInit);
#else
typedef VOID (*pfXcSHAInit) (PUCHAR pbSHAContext);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcSHAUpdate);
#else
typedef VOID (*pfXcSHAUpdate) (PUCHAR pbSHAContext, PUCHAR pbInput, ULONG dwInputLength);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcSHAFinal);
#else
typedef VOID (*pfXcSHAFinal) (PUCHAR pbSHAContext, PUCHAR pbDigest);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcRC4Key);
#else
typedef VOID (*pfXcRC4Key) (PUCHAR pbKeyStruct, ULONG dwKeyLength, PUCHAR pbKey);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcRC4Crypt);
#else
typedef VOID (*pfXcRC4Crypt) (PUCHAR pbKeyStruct, ULONG dwInputLength, PUCHAR pbInput);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcHMAC);
#else
typedef VOID (*pfXcHMAC) (PUCHAR pbKey, ULONG dwKeyLength, PUCHAR pbInput, ULONG dwInputLength, PUCHAR pbInput2, ULONG dwInputLength2, PUCHAR pbDigest);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcPKEncPublic);
#else
typedef ULONG (*pfXcPKEncPublic) (PUCHAR pbPubKey, PUCHAR pbInput, PUCHAR pbOutput);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcPKDecPrivate);
#else
typedef ULONG (*pfXcPKDecPrivate) (PUCHAR pbPrvKey, PUCHAR pbInput, PUCHAR pbOutput);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcPKGetKeyLen);
#else
typedef ULONG (*pfXcPKGetKeyLen) (PUCHAR pbPubKey);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcVerifyPKCS1Signature);
#else
typedef BOOLEAN (*pfXcVerifyPKCS1Signature) (PUCHAR pbSig, PUCHAR pbPubKey, PUCHAR pbDigest);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcModExp);
#else
typedef ULONG (*pfXcModExp) (PULONG pA, PULONG pB, PULONG pC, PULONG pD, ULONG dwN);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcDESKeyParity);
#else
typedef VOID (*pfXcDESKeyParity) (PUCHAR pbKey, ULONG dwKeyLength);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcKeyTable);
#else
typedef VOID (*pfXcKeyTable) (ULONG dwCipher, PUCHAR pbKeyTable, PUCHAR pbKey);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcBlockCrypt);
#else
typedef VOID (*pfXcBlockCrypt) (ULONG dwCipher, PUCHAR pbOutput, PUCHAR pbInput, PUCHAR pbKeyTable, ULONG dwOp);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcBlockCryptCBC);
#else
typedef VOID (*pfXcBlockCryptCBC) (ULONG dwCipher, ULONG dwInputLength, PUCHAR pbOutput, PUCHAR pbInput, PUCHAR pbKeyTable, ULONG dwOp, PUCHAR pbFeedback);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, pfXcCryptService);
#else
typedef ULONG (*pfXcCryptService) (ULONG dwOp, PVOID pArgs);
#endif


typedef struct
{
    pfXcSHAInit pXcSHAInit;
    pfXcSHAUpdate pXcSHAUpdate;
    pfXcSHAFinal pXcSHAFinal;
    pfXcRC4Key pXcRC4Key;
    pfXcRC4Crypt pXcRC4Crypt;
    pfXcHMAC pXcHMAC;
    pfXcPKEncPublic pXcPKEncPublic;
    pfXcPKDecPrivate pXcPKDecPrivate;
    pfXcPKGetKeyLen pXcPKGetKeyLen;
    pfXcVerifyPKCS1Signature pXcVerifyPKCS1Signature;
    pfXcModExp pXcModExp;
    pfXcDESKeyParity pXcDESKeyParity;
    pfXcKeyTable pXcKeyTable;
    pfXcBlockCrypt pXcBlockCrypt;
    pfXcBlockCryptCBC pXcBlockCryptCBC;
    pfXcCryptService pXcCryptService;
} CRYPTO_VECTOR;

DEF_POINTER_TYPE(CRYPTO_VECTOR, PCRYPTO_VECTOR);

typedef struct _OBJECT_HANDLE_TABLE
{
    LONG HandleCount;
    LONG_PTR FirstFreeTableEntry;
    HANDLE NextHandleNeedingPool;
    PPPVOID RootTable;
    PPVOID BuiltinRootTable[8];
} OBJECT_HANDLE_TABLE;

DEF_POINTER_TYPE(OBJECT_HANDLE_TABLE, POBJECT_HANDLE_TABLE);

typedef struct _MMPFNFREE
{
    USHORT PackedPfnFlink;
    USHORT PackedPfnBlink;
} MMPFNFREE;

DEF_POINTER_TYPE(MMPFNFREE, PMMPFNFREE);

typedef struct _MMPFNREGION
{
    MMPFNFREE FreePagesByColor[32];
    PFN_COUNT AvailablePages;
} MMPFNREGION;

DEF_POINTER_TYPE(MMPFNREGION, PMMPFNREGION);

typedef struct _MMPTE
{
    union
    {
        ULONG Long;
        HARDWARE_PTE Hard;
        struct
        {
            ULONG Valid : 1;
            ULONG OneEntry : 1;
            ULONG NextEntry : 30;
        } List;
    };
} MMPTE;

DEF_POINTER_TYPE(MMPTE, PMMPTE);

typedef enum _MMPFN_BUSY_TYPE
{
    MmUnknownUsage,
    MmStackUsage,
    MmVirtualPageTableUsage,
    MmSystemPageTableUsage,
    MmPoolUsage,
    MmVirtualMemoryUsage,
    MmSystemMemoryUsage,
    MmImageUsage,
    MmFsCacheUsage,
    MmContiguousUsage,
    MmDebuggerUsage,
    MmMaximumUsage
} MMPFN_BUSY_TYPE;

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PMMREMOVE_PAGE_ROUTINE);
#else
typedef PFN_NUMBER (FASTCALL *PMMREMOVE_PAGE_ROUTINE) (
    IN MMPFN_BUSY_TYPE BusyType,
    IN PMMPTE TargetPte
);
#endif

typedef struct _MMPTERANGE
{
    MMPTE HeadPte;
    PMMPTE FirstCommittedPte;
    PMMPTE LastCommittedPte;
    PMMPTE LastReservedPte;
    PPFN_COUNT AvailablePages;
    PMMREMOVE_PAGE_ROUTINE RemovePageRoutine;
} MMPTERANGE;

DEF_POINTER_TYPE(MMPTERANGE, PMMPTERANGE);

struct _MMADDRESS_NODE;
DEF_POINTER_TYPE(struct _MMADDRESS_NODE, PMMADDRESS_NODE);
DEF_POINTER_TYPE(PMMADDRESS_NODE, PPMMADDRESS_NODE);

struct _MMADDRESS_NODE
{
    ULONG_PTR StartingVpn;
    ULONG_PTR EndingVpn;
    PMMADDRESS_NODE Parent;
    PMMADDRESS_NODE LeftChild;
    PMMADDRESS_NODE RightChild;
};

typedef struct _MMADDRESS_NODE MMADDRESS_NODE;

typedef struct _MMGLOBALDATA
{
    PMMPFNREGION RetailPfnRegion;
    PMMPTERANGE SystemPteRange;
    PULONG AvailablePages;
    PPFN_COUNT AllocatedPagesByUsage;
    PRTL_CRITICAL_SECTION AddressSpaceLock;
    PPMMADDRESS_NODE VadRoot;
    PPMMADDRESS_NODE VadHint;
    PPMMADDRESS_NODE VadFreeHint;
} MMGLOBALDATA;

DEF_POINTER_TYPE(MMGLOBALDATA, PMMGLOBALDATA);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PIDE_INTERRUPT_ROUTINE);
#else
typedef VOID (*PIDE_INTERRUPT_ROUTINE) (void);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PIDE_FINISHIO_ROUTINE);
#else
typedef VOID (*PIDE_FINISHIO_ROUTINE) (void);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PIDE_POLL_RESET_COMPLETE_ROUTINE);
#else
typedef BOOLEAN (*PIDE_POLL_RESET_COMPLETE_ROUTINE) (void);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PIDE_TIMEOUT_EXPIRED_ROUTINE);
#else
typedef VOID (*PIDE_TIMEOUT_EXPIRED_ROUTINE) (void);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PIDE_START_PACKET_ROUTINE);
#else
typedef VOID (*PIDE_START_PACKET_ROUTINE) (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
);
#endif

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PIDE_START_NEXT_PACKET_ROUTINE);
#else
typedef VOID (*PIDE_START_NEXT_PACKET_ROUTINE) (void);
#endif

typedef struct _IDE_CHANNEL_OBJECT
{
    PIDE_INTERRUPT_ROUTINE InterruptRoutine;
    PIDE_FINISHIO_ROUTINE FinishIoRoutine;
    PIDE_POLL_RESET_COMPLETE_ROUTINE PollResetCompleteRoutine;
    PIDE_TIMEOUT_EXPIRED_ROUTINE TimeoutExpiredRoutine;
    PIDE_START_PACKET_ROUTINE StartPacketRoutine;
    PIDE_START_NEXT_PACKET_ROUTINE StartNextPacketRoutine;
    KIRQL InterruptIrql;
    BOOLEAN ExpectingBusMasterInterrupt;
    BOOLEAN StartPacketBusy;
    BOOLEAN StartPacketRequested;
    UCHAR Timeout;
    UCHAR IoRetries;
    UCHAR MaximumIoRetries;
    PIRP CurrentIrp;
    KDEVICE_QUEUE DeviceQueue;
    ULONG PhysicalRegionDescriptorTablePhysical;
    KDPC TimerDpc;
    KDPC FinishDpc;
    KTIMER Timer;
    KINTERRUPT InterruptObject;
} IDE_CHANNEL_OBJECT;

DEF_POINTER_TYPE(IDE_CHANNEL_OBJECT, PIDE_CHANNEL_OBJECT);

#if DUMB_POINTERS
DEF_POINTER_TYPE(VOID, PKSYSTEM_ROUTINE);
#else
typedef VOID (* NTAPI PKSYSTEM_ROUTINE) (
    IN PKSTART_ROUTINE StartRoutine OPTIONAL,
    IN PVOID StartContext OPTIONAL
);
#endif

// KPRIORITY values
#define LOW_PRIORITY 0              // Lowest thread priority level
#define LOW_REALTIME_PRIORITY 16    // Lowest realtime priority level
#define HIGH_PRIORITY 31            // Highest thread priority level
#define MAXIMUM_PRIORITY 32         // Number of thread priority levels


/* values for CreateDisposition */
/*
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5
*/

/* values for DesiredAccess */
#define DELETE 0x00010000L
#define READ_CONTROL 0x00020000L
#define WRITE_DAC 0x00040000L
#define WRITE_OWNER 0x00080000L
#define SYNCHRONIZE 0x00100000L
#define STANDARD_RIGHTS_REQUIRED 0x000F0000L
#define STANDARD_RIGHTS_READ READ_CONTROL
#define STANDARD_RIGHTS_WRITE READ_CONTROL
#define STANDARD_RIGHTS_EXECUTE READ_CONTROL
#define STANDARD_RIGHTS_ALL 0x001F0000L
#define SPECIFIC_RIGHTS_ALL 0x0000FFFFL
#define ACCESS_SYSTEM_SECURITY 0x01000000L
#define MAXIMUM_ALLOWED 0x02000000L
#define GENERIC_READ 0x80000000L
#define GENERIC_WRITE 0x40000000L
#define GENERIC_EXECUTE 0x20000000L
#define GENERIC_ALL 0x10000000L

#define FILE_READ_DATA 0x0001
#define FILE_LIST_DIRECTORY 0x0001
#define FILE_WRITE_DATA 0x0002
#define FILE_ADD_FILE 0x0002
#define FILE_APPEND_DATA 0x0004
#define FILE_ADD_SUBDIRECTORY 0x0004
#define FILE_CREATE_PIPE_INSTANCE 0x0004
#define FILE_READ_EA 0x0008
#define FILE_WRITE_EA 0x0010
#define FILE_EXECUTE 0x0020
#define FILE_TRAVERSE 0x0020
#define FILE_DELETE_CHILD 0x0040
#define FILE_READ_ATTRIBUTES 0x0080
#define FILE_WRITE_ATTRIBUTES 0x0100
#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1FF)

#define FILE_GENERIC_READ (STANDARD_RIGHTS_READ | FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | SYNCHRONIZE)
#define FILE_GENERIC_WRITE (STANDARD_RIGHTS_WRITE | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_APPEND_DATA | SYNCHRONIZE)
#define FILE_GENERIC_EXECUTE (STANDARD_RIGHTS_EXECUTE | FILE_READ_ATTRIBUTES | FILE_EXECUTE | SYNCHRONIZE)

/* values for FileAttributes */
#define FILE_ATTRIBUTE_READONLY 0x00000001
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_ARCHIVE 0x00000020
#define FILE_ATTRIBUTE_DEVICE 0x00000040
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_TEMPORARY 0x00000100

/* values for CreateOptions */
#define FILE_DIRECTORY_FILE 0x00000001
#define FILE_WRITE_THROUGH 0x00000002
#define FILE_SEQUENTIAL_ONLY 0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING 0x00000008
#define FILE_SYNCHRONOUS_IO_ALERT 0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT 0x00000020
#define FILE_NON_DIRECTORY_FILE 0x00000040
#define FILE_CREATE_TREE_CONNECTION 0x00000080
#define FILE_COMPLETE_IF_OPLOCKED 0x00000100
#define FILE_NO_EA_KNOWLEDGE 0x00000200
#define FILE_OPEN_FOR_RECOVERY 0x00000400
#define FILE_RANDOM_ACCESS 0x00000800
#define FILE_DELETE_ON_CLOSE 0x00001000
#define FILE_OPEN_BY_FILE_ID 0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT 0x00004000
#define FILE_NO_COMPRESSION 0x00008000
#define FILE_RESERVE_OPFILTER 0x00100000
#define FILE_OPEN_REPARSE_POINT 0x00200000
#define FILE_OPEN_NO_RECALL 0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY 0x00800000
#define FILE_COPY_STRUCTURED_STORAGE 0x00000041
#define FILE_STRUCTURED_STORAGE 0x00000441
#define FILE_VALID_OPTION_FLAGS 0x00ffffff
#define FILE_VALID_PIPE_OPTION_FLAGS 0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS 0x00000032
#define FILE_VALID_SET_FLAGS 0x00000036

#define FILE_SUPERSEDE 0x00000000
#define FILE_OPEN 0x00000001
#define FILE_CREATE 0x00000002
#define FILE_OPEN_IF 0x00000003
#define FILE_OVERWRITE 0x00000004
#define FILE_OVERWRITE_IF 0x00000005
#define FILE_MAXIMUM_DISPOSITION 0x00000005

/* values of the IoStatusBlock */
#define FILE_SUPERSEDED 0x00000000
#define FILE_OPENED 0x00000001
#define FILE_CREATED 0x00000002
#define FILE_OVERWRITTEN 0x00000003
#define FILE_EXISTS 0x00000004
#define FILE_DOES_NOT_EXIST 0x00000005

/**
 * Flags for NtAllocateVirtualMemory
 */
#define PAGE_NOACCESS 0x01
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOPY 0x08
#define PAGE_EXECUTE 0x10
#define PAGE_EXECUTE_READ 0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD 0x100
#define PAGE_NOCACHE 0x200
#define PAGE_WRITECOMBINE 0x400
#define PAGE_VIDEO 0x0
#define PAGE_OLD_VIDEO 0x800
#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_DECOMMIT 0x4000
#define MEM_RELEASE 0x8000
#define MEM_FREE 0x10000
#define MEM_PRIVATE 0x20000
#define MEM_MAPPED 0x40000
#define MEM_RESET 0x80000
#define MEM_TOP_DOWN 0x100000
#define MEM_NOZERO 0x800000
#define MEM_LARGE_PAGES 0x20000000
#define MEM_4MB_PAGES 0x80000000

#if defined(__cplusplus)
}
#endif

#pragma ms_struct reset

#pragma clang diagnostic pop
