#pragma once

#include <cstdint>

#include "../defs.h"
#include "sm.h"

namespace openxbox {

// The picture linked below shows:
// https://upload.wikimedia.org/wikipedia/commons/9/94/Xbox-Motherboard-FR.jpg
// PIC16LC63A-04/SO
// (M) 0123857
//
// Producer: http://www.microchip.com/wwwproducts/en/en010145
// Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/30605D.pdf

// NOTE: Instead of calling this device by its real name ("PIC16LC63A-04/SO"),
// we've decided to call this device "SMC", since we don't implement the
// low-level functionality of this PIC, but only the minimum set of high-level
// commands that are sufficient for the Xbox.

// Registers obtained from https://web.archive.org/web/20100617022549/http://www.xbox-linux.org/wiki/PIC:

typedef enum {                   // Read                                        Write
    Version = 0x01,              // PIC version string                          Reset PIC version string counter
    Reset = 0x02,                // -                                           Reset and power off control
    TrayState = 0x03,            // Tray state                                  -
    AVPack = 0x04,               // A/V pack state                              -
    PowerFanMode = 0x05,         // -                                           Power fan mode (0 = automatic; 1 = custom speed from reg 0x06)
    PowerFanRegister = 0x06,     // -                                           Set custom power fan speed (0-50)
    LEDMode = 0x07,              // -                                           LED mode (0 = automatic; 1 = custom sequence from reg 0x08)
    LEDSequence = 0x08,          // -                                           LED flashing sequence
    CPUTemp = 0x09,              // CPU temperature (°C)                        -
    GPUTemp = 0x0A,              // GPU (board?) temperature (°C)               -
    TrayEject = 0x0C,            // -                                           Tray eject (0 = eject; 1 = load)
    ErrorCode = 0x0E,            // -                                           Write error code
    ReadErrorCode = 0x0F,        // Reads error code written with 0x0E          -
    PowerFanReadback = 0x10,     // -                                           Current power fan speed (0-50)
    InterruptReason = 0x11,      // -                                           Interrupt reason
    Overheated = 0x18,           // Locks up the Xbox in "overheated" state     -
    ResetOnEject = 0x19,         // -                                           Reset on eject (0 = enable; 1 = disable)
    InterruptEnable = 0x1A,      // -                                           Interrupt enable (write 0x01 to enable; can't disable once enabled)
    Scratch = 0x1B,              // Scratch register for the original kernel    Scratch register for the original kernel
    Challenge_1C = 0x1C,         // Random number for boot challenge            -
    Challenge_1D = 0x1D,         // Random number for boot challenge            -
    Challenge_1E = 0x1E,         // Random number for boot challenge            -
    Challenge_1F = 0x1F,         // Random number for boot challenge            -
    PICChallenge_20 = 0x20,      // -                                           Response to PIC challenge (written first)
    PICChallenge_21 = 0x21,      // -                                           Response to PIC challenge (written second)
} SMCRegister;


// Register values for SMC_COMMAND_RESET
const uint8_t kSMCReset_AssertReset      = 0x01;
const uint8_t kSMCReset_AssertPowerCycle = 0x40;
const uint8_t kSMCReset_AssertShutdown   = 0x80;

// Register values for SMC_COMMAND_SCRATCH
const uint8_t kSMCScratch_TrayEjectPending  = 0x01;
const uint8_t kSMCScratch_DisplayFatalError = 0x02;
const uint8_t kSMCScratch_ShortAnimation    = 0x04;
const uint8_t kSMCScratch_DashboardBoot     = 0x08;

// http://xboxdevwiki.net/System_Management_Controller
typedef enum {
	P01,
	P2L,
	D01, // Seen in a debug kit 
	D05, // Seen in a earlier model chihiro
} SMCRevision;

// A/V packs, as reported by the SMC
// http://xboxdevwiki.net/AV_Cables
const uint8_t kSMC_AVPack_SCART = 0;     // = AV_PACK_SCART     3
const uint8_t kSMC_AVPack_HDTV = 1;      // = AV_PACK_HDTV      4
const uint8_t kSMC_AVPack_VGA = 2;       // = AV_PACK_VGA       5
const uint8_t kSMC_AVPack_RF = 3;        // = AV_PACK_RFU       2
const uint8_t kSMC_AVPack_SVideo = 4;    // = AV_PACK_SVIDEO    6
const uint8_t kSMC_AVPack_Unknown = 5;   // = AV_PACK_NONE      0
const uint8_t kSMC_AVPack_Standard = 6;  // = AV_PACK_STANDARD  1
const uint8_t kSMC_AVPack_None = 7;      // = AV_PACK_NONE      0


SMCRevision SMCRevisionFromHardwareModel(HardwareModel hardwareModel);


class SMCDevice : public SMDevice {
public:
    // constructor
    SMCDevice(SMCRevision revision);
    virtual ~SMCDevice();

    // SMDevice functions
    void Init();
    void Reset();

    uint8_t GetRegister(SMCRegister reg);

    void QuickCommand(bool read);
    uint8_t ReceiveByte();
    uint8_t ReadByte(uint8_t command);
    uint16_t ReadWord(uint8_t command);
    int ReadBlock(uint8_t command, uint8_t *data);

    void SendByte(uint8_t data);
    void WriteByte(uint8_t command, uint8_t value);
    void WriteWord(uint8_t command, uint16_t value);
    void WriteBlock(uint8_t command, uint8_t* data, int length);

private:
    SMCRevision m_revision;
    int m_PICVersionStringIndex = 0;
    uint8_t m_buffer[256] = {};
};


}
