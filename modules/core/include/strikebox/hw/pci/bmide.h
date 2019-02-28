// Bus Master PCI IDE Controller emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// Based on the Programming Interface for Bus Master IDE Controller
// Revision 1.0 available at http://www.bswd.com/idems100.pdf
// and complemented with information from PC87415 at
// https://parisc.wiki.kernel.org/images-parisc/0/0a/PC87415.pdf
#pragma once

#include <cstdint>
#include <mutex>

#include "../defs.h"
#include "pci.h"
#include "bmide_defs.h"
#include "bmide_channel.h"
#include "strikebox/hw/ata/ata.h"

namespace strikebox {
namespace hw {
namespace bmide {

class BMIDEDevice : public PCIDevice {
public:
    // constructor
    BMIDEDevice(uint8_t *ram, uint32_t ramSize, hw::ata::ATA& ata);
    virtual ~BMIDEDevice();

    // PCI Device functions
    void Init();
    void Reset();

    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;
private:
    BMIDEChannel *m_channels[2];
};

}
}
}
