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

void LPCDevice::PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) {
    log_spew("LPCDevice::PCIIORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);
	
	if (barIndex != 0) {
        log_spew("LPCDevice::PCIIORead:   Unhandled BAR access: %d,  port = 0x%x,  size = %d\n", barIndex, port, size);
        *value = 0;
        return;
	}

    // TODO
	switch (port) {
	case 0x8008: { // TODO: Move 0x8008 TIMER to a device
		if (size == sizeof(uint32_t)) {
			// This timer counts at 3375000 Hz
			auto t = std::chrono::high_resolution_clock::now();
			*value = static_cast<uint32_t>(t.time_since_epoch().count() * 0.003375000);
            return;
		}
		break;
	}
	case 0x80C0: { // TODO: Move 0x80C0 TV encoder to a device
		if (size == sizeof(uint8_t)) {
			// field pin from tv encoder?
			m_field_pin = (m_field_pin + 1) & 1;
			*value = m_field_pin << 5;
            return;
		}
		break;
	}
	}

	log_warning("LPCDevice::PCIIORead:  Unimplemented!  bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);
}

void LPCDevice::PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) {
	//log_spew("LPCDevice::PCIIOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

    // TODO
	log_warning("LPCDevice::PCIIOWrite: Unimplemented!  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);
}

}
