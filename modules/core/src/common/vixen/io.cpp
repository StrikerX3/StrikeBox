#include "vixen/io.h"
#include "vixen/log.h"

namespace vixen {

// ----- Default I/O device implementation ------------------------------------

bool IODevice::IORead(uint32_t addr, uint32_t *value, uint8_t size) {
    *value = 0;
    return false;
}

bool IODevice::IOWrite(uint32_t addr, uint32_t value, uint8_t size) {
    return false;
}

bool IODevice::MMIORead(uint32_t addr, uint32_t *value, uint8_t size) {
    *value = 0;
    return false;
}

bool IODevice::MMIOWrite(uint32_t addr, uint32_t value, uint8_t size) {
    return false;
}

// ----- I/O mapper -----------------------------------------------------------

bool IOMapper::MapIODevice(uint32_t basePort, uint32_t numPorts, IODevice *device) {
    return MapDevice(m_mappedIODevices, basePort, numPorts, device);
}

bool IOMapper::MapMMIODevice(uint32_t baseAddress, uint32_t numAddresses, IODevice *device) {
    return MapDevice(m_mappedMMIODevices, baseAddress, numAddresses, device);
}

bool IOMapper::MapDevice(std::map<uint32_t, MappedDevice>& iomap, uint32_t base, uint32_t size, IODevice *device) {
    uint32_t last = base + size - 1;

    // Ensure there are no overlapping ranges
    auto pu = iomap.upper_bound(base);
    if (pu != iomap.end()) {
        if ((base >= pu->first && base <= pu->second.lastAddress) || (last >= pu->first && last <= pu->second.lastAddress)) {
            log_warning("IOMapper::MapDevice: Attempted to map a device to %s range 0x%x..0x%x, but another device is already mapped to range 0x%x..0x%x\n",
                (&iomap == &m_mappedIODevices) ? "I/O" : "MMIO",
                base, last,
                pu->first, pu->second.lastAddress);
            return false;
        }
    }

    auto pl = iomap.lower_bound(last);
    if (pl != iomap.begin()) pl--;
    if (pl != iomap.begin()) {
        if ((base >= pl->first && base <= pl->second.lastAddress) || (last >= pl->first && last <= pl->second.lastAddress)) {
            log_warning("IOMapper::MapDevice: Attempted to map a device to %s range 0x%x..0x%x, but another device is already mapped to range 0x%x..0x%x\n",
                (&iomap == &m_mappedIODevices) ? "I/O" : "MMIO",
                base, last,
                pu->first, pu->second.lastAddress);
            return false;
        }
    }

    // Map the device to the specified I/O or MMIO range
    iomap[base] = MappedDevice{ base, last, device };

    return true;
}

bool IOMapper::AddDevice(IODevice *device) {
    return m_dynamicDevices.emplace(device).second;
}

bool IOMapper::LookupDevice(std::map<uint32_t, MappedDevice>& iomap, uint32_t addr, IODevice **device) {
    auto p = iomap.upper_bound(addr);

    // p->first > addr
    if (p == iomap.begin()) {
        return false;
    }

    // p->first <= addr
    --p;

    if (addr >= p->first && addr <= p->second.lastAddress) {
        *device = p->second.device;
        return true;
    }

    return false;
}

bool IOMapper::IORead(uint32_t addr, uint32_t *value, uint8_t size) {
    // Try looking up a device mapped to the specified port first
    IODevice *dev;
    if (LookupDevice(m_mappedIODevices, addr, &dev)) {
        return dev->IORead(addr, value, size);
    }

    // Otherwise search for one of the dynamically mapped devices
    for (auto it = m_dynamicDevices.begin(); it != m_dynamicDevices.end(); it++) {
        auto dev = *it;
        if (dev->IORead(addr, value, size)) {
            return true;
        }
    }

    log_warning("IOMapper::IORead:   Unhandled I/O!  address = 0x%x,  size = %u,  read\n", addr, size);
    *value = 0;
    return false;
}

bool IOMapper::IOWrite(uint32_t addr, uint32_t value, uint8_t size) {
    // Try looking up a device mapped to the specified port first
    IODevice *dev;
    if (LookupDevice(m_mappedIODevices, addr, &dev)) {
        return dev->IOWrite(addr, value, size);
    }

    // Otherwise search for one of the dynamically mapped devices
    for (auto it = m_dynamicDevices.begin(); it != m_dynamicDevices.end(); it++) {
        auto dev = *it;
        if (dev->IOWrite(addr, value, size)) {
            return true;
        }
    }

    log_warning("IOMapper::IOWrite:  Unhandled I/O!  address = 0x%x,  size = %u,  write 0x%x\n", addr, size, value);
    return false;
}

bool IOMapper::MMIORead(uint32_t addr, uint32_t *value, uint8_t size) {
    if ((addr & (size - 1)) != 0) {
        log_warning("IOMapper::MMIORead:   Unaligned MMIO read!   address = 0x%x,  size = %u\n", addr, size);
        return false;
    }

    // Try looking up a device mapped to the specified port first
    IODevice *dev;
    if (LookupDevice(m_mappedMMIODevices, addr, &dev)) {
        return dev->MMIORead(addr, value, size);
    }

    // Otherwise search for one of the dynamically mapped devices
    for (auto it = m_dynamicDevices.begin(); it != m_dynamicDevices.end(); it++) {
        auto dev = *it;
        if (dev->MMIORead(addr, value, size)) {
            return true;
        }
    }

    log_warning("IOMapper::MMIORead:   Unhandled MMIO!  address = 0x%x,  size = %u,  read\n", addr, size);
    *value = 0;
    return false;
}

bool IOMapper::MMIOWrite(uint32_t addr, uint32_t value, uint8_t size) {
    if ((addr & (size - 1)) != 0) {
        log_warning("IOMapper::MMIOWrite:  Unaligned MMIO write!  address = 0x%x,  size = %u,  value = 0x%x\n", addr, size, value);
        return false;
    }

    // Try looking up a device mapped to the specified port first
    IODevice *dev;
    if (LookupDevice(m_mappedMMIODevices, addr, &dev)) {
        return dev->MMIOWrite(addr, value, size);
    }

    // Otherwise search for one of the dynamically mapped devices
    for (auto it = m_dynamicDevices.begin(); it != m_dynamicDevices.end(); it++) {
        auto dev = *it;
        if (dev->MMIOWrite(addr, value, size)) {
            return true;
        }
    }

    log_warning("IOMapper::MMIOWrite:  Unhandled MMIO!  address = 0x%x,  size = %u,  write 0x%x\n", addr, size, value);
    return false;
}

}