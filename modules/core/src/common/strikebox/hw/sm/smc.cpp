/*
 * Portions of the code are based on XQEMU's SMBus Xbox System Management Controller emulation.
 * The original copyright header is included below.
 */
/*
 * QEMU SMBus Xbox System Management Controller
 *
 * Copyright (c) 2011 espes
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include "strikebox/hw/sm/smc.h"
#include "strikebox/hw/sm/led.h"

#include "strikebox/log.h"

namespace strikebox {

SMCRevision SMCRevisionFromHardwareModel(HardwareModel hardwareModel) {
    switch (hardwareModel) {
    case Revision1_0:
        return P01; // Our SCM returns PIC version string "P01"
    case Revision1_1:
    case Revision1_2:
    case Revision1_3:
    case Revision1_4:
    case Revision1_5:
    case Revision1_6:
        // EmuWarning("Guessing SCMRevision");
        return P2L; // Assumption; Our SCM returns PIC version string "P05"
    case DebugKit:
        return D01; // Our SCM returns PIC version string "DXB"
    default:
        // UNREACHABLE(hardwareModel);
        return P2L;
    }
}


SMCDevice::SMCDevice(SMCRevision revision) {
    m_revision = revision;
}

SMCDevice::~SMCDevice() {
}

void SMCDevice::Init() {
    m_PICVersionStringIndex = 0;
    memset(m_buffer, 0, sizeof(m_buffer));
    m_buffer[SMCRegister::AVPack] = kSMC_AVPack_HDTV; // see http://xboxdevwiki.net/PIC#The_AV_Pack
    m_buffer[SMCRegister::LEDSequence] = 0xF; // all green
    m_buffer[SMCRegister::Scratch] = 0; // see http://xboxdevwiki.net/PIC#Scratch_register_values
}

void SMCDevice::Reset() {
    log_spew("SMCDevice::Reset:  Unimplemented!\n");
    // TODO
}

uint8_t SMCDevice::GetRegister(SMCRegister reg) {
    return m_buffer[reg];
}

void SMCDevice::QuickCommand(bool read) {
    log_spew("SMCDevice::QuickCommand:  Unimplemented!  %s\n", (read ? "read" : "write"));

    // TODO
}

uint8_t SMCDevice::ReceiveByte() {
    log_spew("SMCDevice::ReceiveByte  Unimplemented!\n");

    return 0; // TODO
}

uint8_t SMCDevice::ReadByte(uint8_t command) {
    log_spew("SMCDevice::ReadByte:  command = 0x%x\n", command);

    switch (command) {
    case SMCRegister::Version:
        // See http://xboxdevwiki.net/PIC#PIC_version_string
        char revByte;
        switch (m_revision) {
        case SMCRevision::P01: revByte = "P01"[m_PICVersionStringIndex]; break;
        case SMCRevision::P2L: revByte = "P05"[m_PICVersionStringIndex]; break; // ??
        case SMCRevision::D01: revByte = "DXB"[m_PICVersionStringIndex]; break;
        case SMCRevision::D05: revByte = "D05"[m_PICVersionStringIndex]; break; // ??
        default: assert(0); revByte = 'P'; break;
        }

        m_PICVersionStringIndex = (m_PICVersionStringIndex + 1) % 3;
        return revByte;
    //case SMCRegister::TrayState:
    //case SMCRegister::AVPack:
    //case SMCRegister::CPUTemp:
    //case SMCRegister::GPUTemp:
    case SMCRegister::ReadErrorCode:
        return m_buffer[SMCRegister::ErrorCode];
    //case SMCRegister::PowerFanReadback:
    //case SMCRegister::InterruptReason:
    //case SMCRegister::Overheated:
    //case SMCRegister::Scratch:

    // See http://xboxdevwiki.net/PIC#PIC_Challenge_.28regs_0x1C.7E0x21.29
    // Taken from https://github.com/xqemu/xqemu/blob/xbox/hw/xbox/smbus_xbox_smc.c
    case SMCRegister::Challenge_1C: return 0x52;
    case SMCRegister::Challenge_1D: return 0x72;
    case SMCRegister::Challenge_1E: return 0xea;
    case SMCRegister::Challenge_1F: return 0x46;
    }

    return m_buffer[command];
}

uint16_t SMCDevice::ReadWord(uint8_t command) {
    log_spew("SMCDevice::ReadWord:  Unimplemented!  command = 0x%x\n", command);

    return 0; // TODO
}

int SMCDevice::ReadBlock(uint8_t command, uint8_t *data) {
    log_spew("SMCDevice::ReadBlock:  Unimplemented!  command = 0x%x\n", command);

    return 0; // TODO
}

void SMCDevice::SendByte(uint8_t data) {
    log_spew("SMCDevice::SendByte:  Unimplemented!  data = 0x%x\n", data);

    // TODO
}

void SMCDevice::WriteByte(uint8_t command, uint8_t value) {
    log_spew("SMCDevice::WriteByte:  command = 0x%x,  value = 0x%x\n", command, value);

    switch (command) {
    case SMCRegister::Version:
        // NOTE: MAME Xbox/Chihiro driver doesn't check for zero
        if (value == 0) {
            m_PICVersionStringIndex = 0;
        }
        return;
    case SMCRegister::Reset:
        // See http://xboxdevwiki.net/PIC#Reset_and_Power_Off
        switch (value) {
        case kSMCReset_AssertReset: return; // TODO
        case kSMCReset_AssertPowerCycle: return; // TODO
        case kSMCReset_AssertShutdown: return; // TODO: Power off, terminating the emulation
        }
    // TODO: case SMCRegister::PowerFanMode:
    // TODO: case SMCRegister::PowerFanRegister:
    case SMCRegister::LEDMode:
        switch (value) {
        case 0: return; // TODO: Automatic LED management
        case 1: return; // TODO: Custom sequence
        default: // TODO:
            // HalWriteSMBusValue(0x20, 0x08, false, x) and then HalWriteSMBusValue(0x20, 0x07, false, y > 1)
            // will cause the led to be solid green, while the next pair of
            // HalWriteSMBusValue with arbitrary y will cause the led to assume the color of the sequence x
            // and afterwards this will repeat with whatever y; ntstatus is always 0
            return;
        }
    case SMCRegister::LEDSequence:
    {
        // Parse and display LED sequence
        LED::Sequence seq = value;
        log_info("SMCDevice::WriteByte: Changed LED flash sequence: %s %s %s %s\n", seq.Name(0), seq.Name(1), seq.Name(2), seq.Name(3));
        break;
    }
    // TODO: case SMCRegister::TrayEject:
    case SMCRegister::ErrorCode:
        log_warning("SMCDevice::WriteByte: Wrote fatal error code %d\n", value);
        
        break;
    // TODO: case SMCRegister::ResetOnEject:
    // TODO: case SMCRegister::InterruptEnable:
    case SMCRegister::Scratch:
        // See http://xboxdevwiki.net/PIC#Scratch_register_values
        switch (value) {
        case kSMCScratch_TrayEjectPending: return; // TODO
        case kSMCScratch_DisplayFatalError: return; // TODO
        case kSMCScratch_ShortAnimation: return; // TODO
        case kSMCScratch_DashboardBoot: return;  // TODO
        }
        break;
    // TODO: case SMCRegister::PICChallenge_20:
    // TODO: case SMCRegister::PICChallenge_21:
    }
    
    m_buffer[command] = value;
}

void SMCDevice::WriteWord(uint8_t command, uint16_t value) {
    log_spew("SMCDevice::WriteWord:  Unimplemented!  command = 0x%x,  value = 0x%x\n", command, value);

    // TODO
}

void SMCDevice::WriteBlock(uint8_t command, uint8_t* data, int length) {
    log_spew("SMCDevice::WriteBlock:  Unimplemented!  command = 0x%x,  length = 0x%x\n", command, length);

    // TODO
}

}
