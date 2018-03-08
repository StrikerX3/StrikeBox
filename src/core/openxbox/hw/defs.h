#pragma once

#include <cstdint>

namespace openxbox {

// Constants for SMBus device addresses.
// The constants point to the write address.
// Reads are done on write address + 1.
const uint8_t kSMBusAddress_MCPX = 0x10;
const uint8_t kSMBusAddress_TVEncoder = 0x88;
const uint8_t kSMBusAddress_SystemMicroController = 0x20;
const uint8_t kSMBusAddress_TemperatureMeasurement = 0x98;
const uint8_t kSMBusAddress_EEPROM = 0xA8;
const uint8_t kSMBusAddress_TVEncoder_ID_Conexant = 0x8A;
const uint8_t kSMBusAddress_TVEncoder_ID_Focus = 0xD4;
const uint8_t kSMBusAddress_TVEncoder_ID_XCalibur = 0xE0;

// Xbox hardware models.
// These affect the configuration of various hardware components.
typedef enum {
    Revision1_0,
    Revision1_1,
    Revision1_2,
    Revision1_3,
    Revision1_4,
    Revision1_5,
    Revision1_6,
    DebugKit
} HardwareModel;

}
