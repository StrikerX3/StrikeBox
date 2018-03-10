#include "lpc.h"
#include "openxbox/log.h"

namespace openxbox {

LPCDevice::LPCDevice(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
	: PCIDevice(PCI_HEADER_TYPE_BRIDGE, vendorID, deviceID, revisionID,
		0x06, 0x01, 0x00, // ISA bridge
        /*TODO: subsystemVendorID*/0x00, /*TODO: subsystemID*/0x00)
{
}

// PCI Device functions

void LPCDevice::Init() {
    RegisterBAR(0, 0x100, PCI_BAR_TYPE_IO); // 0x8000 - 0x80FF
}

void LPCDevice::Reset() {
}

uint32_t LPCDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    //log_spew("LPCDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);
	
	if (barIndex != 0) {
		return 0;
	}

    // TODO
	switch (port) {
	case 0x8008: { // TODO: Move 0x8008 TIMER to a device
		if (size == sizeof(uint32_t)) {
			// This timer counts at 3375000 Hz
			auto t = std::chrono::high_resolution_clock::now();
			return static_cast<uint32_t>(t.time_since_epoch().count() * 0.003375000);
		}
		break;
	}
	case 0x80C0: { // TODO: Move 0x80C0 TV encoder to a device
		if (size == sizeof(uint8_t)) {
			// field pin from tv encoder?
			m_field_pin = (m_field_pin + 1) & 1;
			return m_field_pin << 5;
		}
		break;
	}
	}

	log_warning("LPCDevice::IORead:  Unimplemented!  bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);
    return 0;
}

void LPCDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
	//log_spew("LPCDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
	log_warning("LPCDevice::IOWrite: Unimplemented!  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);
}

uint32_t LPCDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
	//log_spew("LPCDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    // TODO
	log_warning("LPCDevice::MMIORead:  Unimplemented!  bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    return 0;
}

void LPCDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
	//log_spew("LPCDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

    // TODO
    log_warning("LPCDevice::MMIOWrite: Unimplemented!  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);
}

}
