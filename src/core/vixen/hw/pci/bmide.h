// Bus Master PCI IDE Controller emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// Based on the Programming Interface for Bus Master IDE Controller
// Revision 1.0 available at http://www.bswd.com/idems100.pdf
// and complemented with information from PC87415 at
// https://parisc.wiki.kernel.org/images-parisc/0/0a/PC87415.pdf
#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"
#include "bmide/defs.h"

namespace vixen {

class BMIDEDevice : public PCIDevice {
public:
    // constructor
    BMIDEDevice(uint8_t *ram, uint32_t ramSize);
    virtual ~BMIDEDevice();

    // PCI Device functions
    void Init();
    void Reset();

    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;
private:
    // ----- Registers --------------------------------------------------------

    uint8_t m_command[hw::bmide::kNumChannels] = { 0 };
    uint8_t m_status[hw::bmide::kNumChannels] = { 0 };
    uint32_t m_prdTableAddrs[hw::bmide::kNumChannels] = { 0 };

    // ----- System memory ----------------------------------------------------

    uint8_t *m_ram = nullptr;
    uint32_t m_ramSize = 0;

    // ----- Operations -------------------------------------------------------

    void ReadCommand(hw::bmide::Channel channel, uint32_t *value, uint8_t size);
    void ReadStatus(hw::bmide::Channel channel, uint32_t *value, uint8_t size);
    void ReadPRDTableAddress(hw::bmide::Channel channel, uint32_t *value, uint8_t size);

    void WriteCommand(hw::bmide::Channel channel, uint32_t value, uint8_t size);
    void WriteStatus(hw::bmide::Channel channel, uint32_t value, uint8_t size);
    void WritePRDTableAddress(hw::bmide::Channel channel, uint32_t value, uint8_t size);
};

}
