// ATA/ATAPI-4 emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// This code aims to implement a subset of the ATA/ATAPI-4 specification
// that satisifies the requirements of an IDE interface for the Original Xbox.
//
// Specification:
// http://www.t13.org/documents/UploadedDocuments/project/d1153r18-ATA-ATAPI-4.pdf
//
// References to particular items in the specification are denoted between brackets
// optionally followed by a quote from the specification.
#pragma once

#include <cstdint>

namespace vixen {
namespace hw {
namespace ata {

// --- I/O resources ------------------------------------------------------------------------------

// Resources for the primary IDE channel
// IRQ 14     Command ports 0x1F0 - 0x1F7    Control port 0x3F6
const uint8_t  kPrimaryIRQ              = 14;
const uint16_t kPrimaryCommandBasePort  = 0x1f0;
const uint16_t kPrimaryCommandPortCount = 8;
const uint16_t kPrimaryCommandLastPort  = (kPrimaryCommandBasePort + kPrimaryCommandPortCount - 1);
const uint16_t kPrimaryControlPort      = 0x3f6;

// Resources for the secondary IDE channel
// IRQ 15     Command ports 0x170 - 0x177    Control port 0x376
const uint8_t  kSecondaryIRQ              = 15;
const uint16_t kSecondaryCommandBasePort  = 0x170;
const uint16_t kSecondaryCommandPortCount = 8;
const uint16_t kSecondaryCommandLastPort  = (kSecondaryCommandBasePort + kSecondaryCommandPortCount - 1);
const uint16_t kSecondaryControlPort      = 0x376;

// --- Sizes and capacities -----------------------------------------------------------------------

const uint32_t kSectorSize = 512;
const uint32_t kMaxCylinders = 16383;
const uint32_t kMaxHeads = 16;
const uint32_t kMaxSectorsPerTrack = 63;
const uint32_t kMaxCHSSectorCapacity = kMaxCylinders * kMaxHeads * kMaxSectorsPerTrack;

// --- Registers ----------------------------------------------------------------------------------

// Command port registers [chapter 7]
enum Register : uint8_t {
    RegData = 0,           // [7.7]  Read/write 
    RegError = 1,          // [7.11] Read-only  
    RegFeatures = 1,       // [7.12] Write-only 
    RegSectorCount = 2,    // [7.13] Read/write 
    RegSectorNumber = 3,   // [7.14] Read/write 
    RegCylinderLow = 4,    // [7.6]  Read/write 
    RegCylinderHigh = 5,   // [7.5]  Read/write 
    RegDeviceHead = 6,     // [7.10] Read/write 
    RegStatus = 7,         // [7.15] Read-only  
    RegCommand = 7,        // [7.4]  Write-only 
};

// Register sizes
const uint8_t kRegSize8Bit = 0x01;
const uint8_t kRegSize16Bit = 0x02;
const uint8_t kRegSize32Bit = 0x04;

const uint8_t kRegSizes[] = {
    kRegSize8Bit | kRegSize16Bit | kRegSize32Bit,   // [7.7]  Data
    kRegSize8Bit,   // [7.11] Error and [7.12] Features
    kRegSize8Bit,   // [7.13] Sector count
    kRegSize8Bit,   // [7.14] Sector number
    kRegSize8Bit,   // [7.6]  Cylinder low
    kRegSize8Bit,   // [7.6]  Cylinder high
    kRegSize8Bit,   // [7.10] Device/Head
    kRegSize8Bit,   // [7.15] Status and [7.4] Command
};

// Control port registers:
//   Alternate Status when reading  [7.3]
//   Device Control when writing    [7.9]

// There also exists a Data port that is used with DMA transfers
// and cannot be directly accessed through port I/O.  [7.8]

// --- Register data bits -------------------------------------------------------------------------

// Status bits (read from the Status register)
enum StatusBits : uint8_t {
    StBusy = (1 << 7),          // [7.15.6.1] (BSY) The device is busy
    StReady = (1 << 6),         // [7.15.6.2] (DRDY) The device is ready to accept commands
    StDeviceFault = (1 << 5),   // [--------] (DF) Device fault  ([7.15.6.3] says that this is a command dependent bit)
    StBit4 = (1 << 4),          // [7.15.6.3] Command dependent bit
    StDataRequest = (1 << 3),   // [7.15.6.4] (DRQ) The device is ready to transfer a word of data
    StError = (1 << 0),         // [7.15.6.6] (ERR) An error occurred during execution of the previous command
};

// Error bits (read from the Error register)
enum ErrorBits : uint8_t {
    ErrAbort = (1 << 2),        // [7.11.6] (ABRT) Previous command was aborted due to an error or invalid parameter
    
    // [8.23.6] Read DMA and [8.45.6] Write DMA error flags
    ErrDMACRC = (1 << 7),                  // (ICRC) CRC error occurred during Ultra DMA transfer
    ErrDMAUncorrectable = (1 << 6),        // (UNC) Data is uncorrectable
    ErrDMARemMediaChanged = (1 << 5),      // (MC) Removable media changed since last command was issued
    ErrDMADataNotFound = (1 << 4),         // (IDNF) Tried to read outside user-accessible range
    ErrDMAMediaChangeRequest = (1 << 2),   // (MCR) Media changed requested during the operation
    ErrDMANoMedia = (1 << 1),              // (NM) No media is present in removable media device
};

// Device control bits (written to the Device Control register)
enum DeviceControlBits : uint8_t {
    DevCtlSoftwareReset = (1 << 2),          // [7.9.6] (SRST) Execute a software reset
    DevCtlNegateInterruptEnable = (1 << 1),  // [7.9.6] (nIEN) When set, INTRQ signal is effectively disabled
};

// Device/Head bits
enum DeviceHeadBits : uint8_t {
    DevHeadDMALBA = (1 << 6),   // [8.23.4 and 8.45.4] On DMA operations, determines if the operation will use LBA address or CHS numbers
};

const uint8_t kDevSelectorBit = 4;  // [7.10.6] (DEV) Selects Device 0 when cleared or Device 1 when set

// --- Transfer modes -----------------------------------------------------------------------------

// [8.37.10 table 20] PIO transfer types for the Set Transfer Mode subcommand of the Set Features command.
// These specify the 5 most significant bits of the transfer mode value.
// The 3 least significant bits specify additional parameters for the mode. When applicable, they are documented below.
enum PIOTransferType : uint8_t {
    XferTypePIODefault = 0b00000,     // PIO default mode:
                                      //   000b = IORDY enabled
                                      //   001b = IORDY disabled
    XferTypePIOFlowCtl = 0b00001,     // PIO flow control transfer mode
                                      //   LSB specify mode (0 to 4)
};

// The highest supported PIO flow control transfer mode
const uint8_t kMaximumPIOTransferMode = 4;


// [8.37.10 table 20] DMA transfer types for the Set Transfer Mode subcommand of the Set Features command.
enum DMATransferType : uint8_t {
    XferTypeMultiWordDMA = 0b00100,   // Multiword DMA mode
                                      //   LSB specify mode (0 to 2)
    XferTypeUltraDMA = 0b01000,       // Ultra DMA mode
                                      //   LSB specify mode (0 to 2)
};

// The highest supported Multiword DMA transfer mode
const uint8_t kMaximumMultiwordDMATransferMode = 2;

// The highest supported Ultra DMA transfer mode
const uint8_t kMaximumUltraDMATransferMode = 4;

// --- Commands -----------------------------------------------------------------------------------

// [8] Commands
enum Command : uint8_t {
    CmdDeviceReset = 0x08,                  // [8.7]  Device Reset
    CmdIdentifyDevice = 0xEC,               // [8.12] Identify Device
    CmdInitializeDeviceParameters = 0x91,   // [8.16] Initialize Device Parameters
    CmdReadDMA = 0xC8,                      // [8.23] Read DMA
    CmdSecurityUnlock = 0xF2,               // [8.34] Security Unlock
    CmdSetFeatures = 0xEF,                  // [8.37] Set Features
    CmdWriteDMA = 0xCA,                     // [8.45] Write DMA
};

// [8.37.8] Set Features subcommands (specified in the Features register)
enum SetFeaturesSubCommand : uint8_t {
    SFCmdSetTransferMode = 0x03,   // [8.37.10] Set Transfer Mode
};

// --- Command Protocols --------------------------------------------------------------------------

// [9] Command Protocols
// This struct specifies the behavior of commands that follow a particular protocol.
// Flags and the INTRQ are asserted or negated depending on the outcome.
// The BSY status flag is automatically managed by the code; it will be ignored if specified here.
struct CommandProtocol {
    uint8_t statusAssertedOnSuccess;   // Status flags asserted on successful command execution
    uint8_t statusNegatedOnError;      // Status flags negated on failed command execution
    bool assertINTRQOnSuccess;         // Command triggers INTRQ on success
};

// NOTE: Not all command protocols are included here.

const CommandProtocol kCmdProtoDeviceReset = { 0, 0, false };                    // [9.2]  Device reset  (hardware reset)
const CommandProtocol kCmdProtoPIODataIn = { StDataRequest, 0, true };           // [9.7]  PIO data in   (data transfer from device to host via Data register)
const CommandProtocol kCmdProtoPIODataOut = { StDataRequest, 0, false };         // [9.8]  PIO data out  (data transfer from host to device via Data register)
const CommandProtocol kCmdProtoNonData = { 0, 0, true, };                        // [9.9]  Non-data      (no data transfer)
const CommandProtocol kCmdProtoDMA = { StDataRequest, StDataRequest, false, };   // [9.10] DMA           (data transfer between host and device via DMA)
// TODO: CmdProtoPACKET  // [9.11] PACKET        (non-data, PIO and DMA transfers)

// Map commands to their protocols
const std::unordered_map<Command, const CommandProtocol&, std::hash<uint8_t>> kCmdProtocols = {
    { CmdDeviceReset, kCmdProtoDeviceReset },
    { CmdIdentifyDevice, kCmdProtoPIODataIn },
    { CmdInitializeDeviceParameters, kCmdProtoNonData },
    { CmdReadDMA, kCmdProtoDMA },
    { CmdSecurityUnlock, kCmdProtoPIODataOut },
    { CmdSetFeatures, kCmdProtoNonData },
    { CmdWriteDMA, kCmdProtoDMA },
};

// --- Command data -------------------------------------------------------------------------------

// [8.12.8] Length of the data structure returned by the Identify Device command, in words
const uint16_t kIdentifyDeviceWords = 256;

// [8.12.8 table 11 words 10-19] Length of the serial number field
const uint8_t kSerialNumberLength = 20;

// [8.12.8 table 11 words 23-26] Length of the firmware revision field
const uint8_t kFirmwareRevLength = 8;

// [8.12.8 table 11 words 27-46] Length of the model number field
const uint8_t kModelNumberLength = 40;

// [8.12.8 table 11] Data returned by the Identify Device command
#ifdef _MSC_VER
#include <pshpack2.h>
#endif
struct IdentifyDeviceData {
    uint16_t generalConfiguration;                   // word 0
    uint16_t numLogicalCylinders;                    // word 1
    uint16_t _reserved_1;                            // word 2
    uint16_t numLogicalHeads;                        // word 3
    uint16_t _retired_1[2];                          // word 4-5
    uint16_t numLogicalSectorsPerTrack;              // word 6
    uint16_t _retired_2[3];                          // word 7-9
    char     serialNumber[kSerialNumberLength];      // word 10-19
    uint16_t _retired_3[2];                          // word 20-21
    uint16_t _obsolete_1;                            // word 22
    char     firmwareRevision[kFirmwareRevLength];   // word 23-26
    char     modelNumber[kModelNumberLength];        // word 27-46
    uint16_t maxTransferPerInterrupt : 8;            // word 47
    uint16_t _reserved_2 : 8;
    uint16_t _reserved_3;                            // word 48
    uint16_t capabilities1;                          // word 49
    uint16_t capabilities2;                          // word 50
    uint16_t pioDataTransferModeNumber : 8;          // word 51
    uint16_t _retired_4 : 8;
    uint16_t _retired_5;                             // word 52
    uint16_t validTranslationFields;                 // word 53
    uint16_t numCurrentLogicalCylinders;             // word 54
    uint16_t numCurrentLogicalHeads;                 // word 55
    uint16_t numCurrentLogicalSectorsPerTrack;       // word 56
    uint32_t currentSectorCapacity;                  // word 57-58
    uint16_t currentMultiSectorSettings;             // word 59
    uint32_t numAddressableSectors;                  // word 60-61
    uint16_t _retired_6;                             // word 62
    uint16_t multiwordDMASettings;                   // word 63
    uint16_t advancedPIOModesSupported : 8;          // word 64
    uint16_t _reserved_4 : 8;
    uint16_t minMDMATransferCyclePerWord;            // word 65
    uint16_t recommendedMDMATransferCycleTime;       // word 66
    uint16_t minPIOTransferCycleNoFlowCtl;           // word 67
    uint16_t minPIOTransferCycleIORDYFlowCtl;        // word 68
    uint16_t _reserved_5[2];                         // word 69-70
    uint16_t _reserved_6[4];                         // word 71-74
    uint16_t queueDepth : 5;                         // word 75
    uint16_t _reserved_7 : 11;
    uint16_t _reserved_8[4];                         // word 76-79
    uint16_t majorVersionNumber;                     // word 80
    uint16_t minorVersionNumber;                     // word 81
    uint16_t commandSetsSupported1;                  // word 82
    uint16_t commandSetsSupported2;                  // word 83
    uint16_t commandSetsSupported3;                  // word 84
    uint16_t commandSetsEnabled1;                    // word 85
    uint16_t commandSetsEnabled2;                    // word 86
    uint16_t commandSetsEnabled3;                    // word 87
    uint16_t ultraDMASettings;                       // word 88
    uint16_t timeToCompleteSecurityErase;            // word 89
    uint16_t timeToCompleteEnhancedSecurityErase;    // word 90
    uint16_t currentAdvancedPowerMgmtValue;          // word 91
    uint16_t _reserved_9[35];                        // word 92-126
    uint16_t remMediaStatusNotificationFeatureSets;  // word 127
    uint16_t securityStatus;                         // word 128
    uint16_t _vendor[31];                            // word 129-159
    uint16_t _reserved_10[96];                       // word 160-255
#ifdef _MSC_VER
};
#include <poppack.h>
#else
} __attribute__((aligned(2),packed));
#endif

// [8.12.8 table 11 word 0] Bits for the generalConfiguration field in the IdentifyDeviceData struct
enum IdentifyDeviceGeneralConfiguration : uint16_t {
    IDGenConfATADevice = (1 << 15),              // ATA device
    IDGenConfRemovableMedia = (1 << 7),          // Removable media device
    IDGenConfNotRemovableController = (1 << 6),  // Not removable controller and/or device
};

// [8.12.8 table 11 word 49] Bits for the capabilities1 field in the IdentifyDeviceData struct
enum IdentifyDeviceCapabilities1 : uint16_t {
    IDCaps1StanbyTimerValuesSupported = (1 << 13),   // Standby timer values as specified in the standard are supported
    IDCaps1IORDYSupported = (1 << 11),               // IORDY supported
    IDCaps1CanDisableIODRY = (1 << 10),              // IORDY may be disabled
};

// [8.12.8 table 11 word 50] Bits for the capabilities2 field in the IdentifyDeviceData struct
enum IdentifyDeviceCapabilities2 : uint16_t {
    IDCaps2Bit15AlwaysZero = (1 << 15),   // This bit must always be zero
    IDCaps2Bit14AlwaysOne = (1 << 14),    // This bit must always be one
    IDCaps2DeviceSpecificStandbyTimerMinimum = (1 << 0),   // There is a device specific Standby timer value minimum
};

// [8.12.8 table 11 word 53] Bits for the validTranslationFields field in the IdentifyDeviceData struct
enum IdentifyDeviceValidTranslationFields : uint16_t {
    IDValidXlatUltraDMA = (1 << 2),           // The fields reported in word 88 are valid -> supported and enabled Ultra DMA modes
    IDValidXlatTransferCycles = (1 << 1),     // The fields reported in words 64-70 are valid -> transfer cycles and advanced PIO modes supported
    IDValidXlatCHS = (1 << 0),                // The fields reported in words 54-58 are valid -> CHS information
};

// [8.12.8 table 11 word 59] Bits for the currentMultiSectorSettings field in the IdentifyDeviceData struct
enum IdentifyDeviceCurrentMultiSectorSettings : uint16_t {
    IDMultiSectValid = (1 << 8),              // Multiple sector setting is valid
    IDMultiSectNumSectorsPerInterrupt = 0xFF, // Number of sectors that can be transferred using read/write multiple commands
};

// [8.12.8 table 11 word 63] Bits for the multiwordDMASettings field in the IdentifyDeviceData struct
enum IdentifyDeviceMultiwordDMASettings : uint16_t {
    IDMultiwordDMA2Selected = (1 << 10),    // Multiword DMA mode 2 selected
    IDMultiwordDMA1Selected = (1 << 9),     // Multiword DMA mode 1 selected
    IDMultiwordDMA0Selected = (1 << 8),     // Multiword DMA mode 0 selected
    IDMultiwordDMA2Supported = (1 << 2),    // Multiword DMA modes 2 and below are supported
    IDMultiwordDMA1Supported = (1 << 1),    // Multiword DMA modes 1 and below are supported
    IDMultiwordDMA0Supported = (1 << 0),    // Multiword DMA mode 0 is supported
};

// [8.12.8 table 11 word 80] Bits for the majorVersionNumber field in the IdentifyDeviceData struct
enum IdentifyDeviceMajorVersionNumber : uint16_t {
    IDMajorVerNoReport = 0,          // Device does not report version
    IDMajorVerNoReportAlt = 0xFFFF,  // Device does not report version
    IDMajorVerATA1 = (1 << 1),       // Supports ATA-1
    IDMajorVerATA2 = (1 << 2),       // Supports ATA-2
    IDMajorVerATA3 = (1 << 3),       // Supports ATA-3
    IDMajorVerATAPI4 = (1 << 4),     // Supports ATA/ATAPI-4
    IDMajorVerATAPI5 = (1 << 5),     // Reserved for ATA/ATAPI-5
    IDMajorVerATAPI6 = (1 << 6),     // Reserved for ATA/ATAPI-6
    IDMajorVerATAPI7 = (1 << 7),     // Reserved for ATA/ATAPI-7
    IDMajorVerATAPI8 = (1 << 8),     // Reserved for ATA/ATAPI-8
    IDMajorVerATAPI9 = (1 << 9),     // Reserved for ATA/ATAPI-9
    IDMajorVerATAPI10 = (1 << 10),   // Reserved for ATA/ATAPI-10
    IDMajorVerATAPI11 = (1 << 11),   // Reserved for ATA/ATAPI-11
    IDMajorVerATAPI12 = (1 << 12),   // Reserved for ATA/ATAPI-12
    IDMajorVerATAPI13 = (1 << 13),   // Reserved for ATA/ATAPI-13
    IDMajorVerATAPI14 = (1 << 14),   // Reserved for ATA/ATAPI-14
    IDMajorVerATAPI15 = (1 << 15),   // Reserved for ATA/ATAPI-15
};

// [8.12.8 table 11 word 81] Bits for the minorVersionNumber field in the IdentifyDeviceData struct
// (specified in [8.12.44 table 12])
enum IdentifyDeviceMinorVersionNumber : uint16_t {
    IDMinorVerNoReport = 0,                         // Device does not report version
    IDMinorVerNoReportAlt = 0xFFFF,                 // Device does not report version
    IDMinorVerATA1_X3T9_2_781D_pre_rev4 = 0x0001,   // ATA (ATA-1) X3T9.2 781D prior to revision 4
    IDMinorVerATA1_ANSI_X3_221_1994 = 0x0002,       // ATA-1 published, ANSI X3.221-1994
    IDMinorVerATA1_X3T10_781D_rev4 = 0x0003,        // ATA (ATA-1) X3T10 781D revision 4
    IDMinorVerATA2_ANSI_X3_279_1996 = 0x0004,       // ATA-2 published, ANSI X3.279-1996
    IDMinorVerATA2_X3T10_948D_pre_rev2k = 0x0005,   // ATA-2 X3T10 948D prior to revision 2k
    IDMinorVerATA3_X3T10_2008D_rev1 = 0x0006,       // ATA-3 X3T10 2008D revision 1
    IDMinorVerATA2_X3T10_948D_rev2k = 0x0007,       // ATA-2 X3T10 948D revision 2k
    IDMinorVerATA3_X3T10_2008D_rev0 = 0x0008,       // ATA-3 X3T10 2008D revision 0
    IDMinorVerATA2_X3T10_948D_rev3 = 0x0009,        // ATA-2 X3T10 948D revision 3
    IDMinorVerATA3_ANSI_X3_298_199x = 0x000a,       // ATA-3 published, ANSI X3.298-199x
    IDMinorVerATA3_X3T10_2008D_rev6 = 0x000b,       // ATA-3 X3T10 2008D revision 6
    IDMinorVerATA3_X3T10_2008D_rev7_7a = 0x000c,    // ATA-3 X3T13 2008D revision 7 and 7a
    IDMinorVerATAPI4_X3T13_1153D_rev6 = 0x000d,     // ATA/ATAPI-4 X3T13 1153D revision 6
    IDMinorVerATAPI4_T13_1153D_rev13 = 0x000e,      // ATA/ATAPI-4 T13 1153D revision 13
    IDMinorVerATAPI4_X3T13_1153D_rev7 = 0x000f,     // ATA/ATAPI-4 X3T13 1153D revision 7
    IDMinorVerATAPI4_T13_1153D_rev15 = 0x0011,      // ATA/ATAPI-4 T13 1153D revision 15
    IDMinorVerATAPI4_T13_1153D_rev14 = 0x0014,      // ATA/ATAPI-4 T13 1153D revision 14
    IDMinorVerATAPI4_T13_1153D_rev17 = 0x0017,      // ATA/ATAPI-4 T13 1153D revision 17
};

// [8.12.8 table 11 words 82 and 85] Bits for the commandSetsSupported1 and commandSetsEnabled1 fields in the IdentifyDeviceData struct
enum IdentifyDeviceCommandSets1 : uint16_t {
    IDCmdSet1NoReport = 0x0000,                     // Command set notification not supported
    IDCmdSet1NoReportAlt = 0xFFFF,                  // Command set notification not supported
    IDCmdSet1NOP = (1 << 14),                       // NOP command supported
    IDCmdSet1ReadBuffer = (1 << 13),                // Read Buffer command supported
    IDCmdSet1WriteBuffer = (1 << 12),               // Write Buffer command supported
    IDCmdSet1HostProtectedArea = (1 << 10),         // Host Protected Area feature set supported
    IDCmdSet1DeviceReset = (1 << 9),                // Device Reset command supported
    IDCmdSet1ServiceInterrupt = (1 << 8),           // Service interrupt supported
    IDCmdSet1ReleaseInterrupt = (1 << 7),           // Release interrupt supported
    IDCmdSet1LookAhead = (1 << 6),                  // Look-ahead supported
    IDCmdSet1WriteCache = (1 << 5),                 // Write cache supported
    IDCmdSet1PacketCommandFeatureSet = (1 << 4),    // PACKET Command feature set supported
    IDCmdSet1PowerMgmtFeatureSet = (1 << 3),        // Power Management feature set supported
    IDCmdSet1RemovableMediaFeatureSet = (1 << 2),   // Removable Media feature set supported
    IDCmdSet1SecurityModeFeatureSet = (1 << 1),     // Security Mode feature set supported
    IDCmdSet1SMARTFeatureSet = (1 << 0),            // SMART feature set supported
};

// [8.12.8 table 11 words 83 and 86] Bits for the commandSetsSupported2 and commandSetsEnabled2 fields in the IdentifyDeviceData struct
enum IdentifyDeviceCommandSets2 : uint16_t {
    IDCmdSet2NoReport = 0x0000,                                 // Command set notification not supported
    IDCmdSet2NoReportAlt = 0xFFFF,                              // Command set notification not supported
    IDCmdSet2Bit15AlwaysZero = (1 << 15),                       // This bit must always be zero
    IDCmdSet2Bit14AlwaysOne = (1 << 14),                        // This bit must always be one
    IDCmdSet2RemMediaStatusNotificationFeatureSet = (1 << 4),   // Removable Media Status Notification feature set supported
    IDCmdSet2AdvancedPowerMgmtFeatureSet = (1 << 3),            // Advanced Power Management feature set supported
    IDCmdSet2CFAFeatureSet = (1 << 2),                          // CFA feature set supported
    IDCmdSet2ReadWriteDMAQueued = (1 << 1),                     // READ/WRITE DMA QUEUED supported
    IDCmdSet2DownloadMicrocode = (1 << 0),                      // DOWNLOAD MICROCODE command supported
};

// [8.12.8 table 11 words 84 and 87] Bits for the commandSetsSupported3 and commandSetsEnabled3 fields in the IdentifyDeviceData struct
enum IdentifyDeviceCommandSets3 : uint16_t {
    IDCmdSet3NoReport = 0x0000,             // Command set notification not supported
    IDCmdSet3NoReportAlt = 0xFFFF,          // Command set notification not supported
    IDCmdSet3Bit15AlwaysZero = (1 << 15),   // This bit must always be zero
    IDCmdSet3Bit14AlwaysOne = (1 << 14),    // This bit must always be one
};

// [8.12.8 table 11 word 88] Bits for the ultraDMASettings field in the IdentifyDeviceData struct
enum IdentifyDeviceUltraDMASettings : uint16_t {
    IDUltraDMA2Selected = (1 << 10),    // Ultra DMA mode 2 selected
    IDUltraDMA1Selected = (1 << 9),     // Ultra DMA mode 1 selected
    IDUltraDMA0Selected = (1 << 8),     // Ultra DMA mode 0 selected
    IDUltraDMA2Supported = (1 << 2),    // Ultra DMA modes 2 and below are supported
    IDUltraDMA1Supported = (1 << 1),    // Ultra DMA modes 1 and below are supported
    IDUltraDMA0Supported = (1 << 0),    // Ultra DMA mode 0 is supported
};

// [8.12.8 table 11 word 127] Bits for the remMediaStatusNotificationFeatureSets field in the IdentifyDeviceData struct
enum IdentifyDeviceRemovableMediaStatus : uint16_t {
    IDRemMediaStatusNotSupported = 0b00,   // Removable Media Status Notification feature set not supported
    IDRemMediaStatusSupported = 0b01,      // Removable Media Status Notification feature set supported
};

// [8.12.8 table 11 word 128] Bits for the securityStatus field in the IdentifyDeviceData struct
enum IdentifyDeviceSecurityStatus : uint16_t {
    IDSecStatusSecurityLevel = (1 << 8),   // Security level (0 = high, 1 = maximum)
    IDSecStatusEnhancedSecurityEraseSupported = (1 << 5),   // Enhanced security erase supported
    IDSecStatusCountExpired = (1 << 4),    // Security count expired
    IDSecStatusFrozen = (1 << 3),          // Security frozen
    IDSecStatusLocked = (1 << 2),          // Security locked
    IDSecStatusEnabled = (1 << 1),         // Security enabled
    IDSecStatusSupported = (1 << 0),       // Security supported
};

}
}
}
