#include "mcpx_ram.h"
#include "openxbox/log.h"

namespace openxbox {

MCPXRAMDevice::MCPXRAMDevice(MCPXRevision revision)
	: PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x02A6, 0xA1,
		0x05, 0x00, 0x00) // RAM controller
{
    m_revision = revision;
}

// PCI Device functions

void MCPXRAMDevice::Init() {
}

void MCPXRAMDevice::Reset() {
}

uint32_t MCPXRAMDevice::IORead(int barIndex, uint32_t port, unsigned size) {
    //log_spew("MCPXRAMDevice::IORead:   bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

	// TODO
	log_warning("MCPXRAMDevice::IORead:  Unimplemented!  bar = %d,  port = 0x%x,  size = %d\n", barIndex, port, size);

    return 0;
}

void MCPXRAMDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {
    //log_spew("MCPXRAMDevice::IOWrite:  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);

	// TODO
	log_warning("MCPXRAMDevice::IOWrite: Unimplemented!  bar = %d,  port = 0x%x,  size = %d,  value = 0x%x\n", barIndex, port, size, value);
}

uint32_t MCPXRAMDevice::MMIORead(int barIndex, uint32_t addr, unsigned size) {
    //log_spew("MCPXRAMDevice::MMIORead:   bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

	// TODO
	log_warning("MCPXRAMDevice::MMIORead:  Unimplemented!  bar = %d,  addr = 0x%x,  size = %d\n", barIndex, addr, size);

    return 0;
}

void MCPXRAMDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) {
    //log_spew("MCPXRAMDevice::MMIOWrite:  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);

	// TODO
	log_warning("MCPXRAMDevice::MMIOWrite: Unimplemented!  bar = %d,  addr = 0x%x,  size = %d,  value = 0x%x\n", barIndex, addr, size, value);
}

}
