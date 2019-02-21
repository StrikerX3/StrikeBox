#pragma once

#include <cstdint>
#include <map>
#include <set>

namespace vixen {

class IOMapper;

/*!
 * Abstract base class that represents devices that respond to port-mapped
 * and/or memory-mapped I/O.
 *
 * I/O functions return true if the I/O operation was handled by the device.
 *
 * The default implementations read the value 0 and don't handle I/O.
 */
class IODevice {
public:
    virtual bool MapIO(IOMapper *mapper) = 0;

    virtual bool IORead(uint32_t port, uint32_t *value, uint8_t size);
    virtual bool IOWrite(uint32_t port, uint32_t value, uint8_t size);

    virtual bool MMIORead(uint32_t addr, uint32_t *value, uint8_t size);
    virtual bool MMIOWrite(uint32_t addr, uint32_t value, uint8_t size);
};

/*!
 * A mapped I/O device with specified I/O or MMIO ranges.
 */
struct MappedDevice {
    // I/O or MMIO addresses
    uint32_t baseAddress;
    uint32_t lastAddress;
    
    // The device itself
    IODevice *device;
};

/*!
 * Maps I/O and MMIO reads and writes to the corresponding devices.
 */
class IOMapper {
public:
    /*!
    * Maps a device to the specified range of ports.
    */
    bool MapIODevice(uint32_t basePort, uint32_t numPorts, IODevice *device);
    
    /*!
     * Maps a device to the specified MMIO range.
     */
    bool MapMMIODevice(uint32_t baseAddress, uint32_t numAddresses, IODevice *device);

    /*!
     * Adds a device with dynamic I/O address mapping.
     */
    bool AddDevice(IODevice *device);

    bool IORead(uint32_t addr, uint32_t *value, uint8_t size);
    bool IOWrite(uint32_t addr, uint32_t value, uint8_t size);

    bool MMIORead(uint32_t addr, uint32_t *value, uint8_t size);
    bool MMIOWrite(uint32_t addr, uint32_t value, uint8_t size);

private:
    /*!
     * Looks up the I/O device mapped to the specified I/O or MMIO address,
     * depending on the map used.
     *
     * Returns true if found.
     */
    bool LookupDevice(std::map<uint32_t, MappedDevice>& iomap, uint32_t addr, IODevice **device);

    /*!
     * Maps a device to the specified address range.
     */
    bool MapDevice(std::map<uint32_t, MappedDevice>& iomap, uint32_t base, uint32_t size, IODevice *device);

    std::map<uint32_t, MappedDevice> m_mappedIODevices;
    std::map<uint32_t, MappedDevice> m_mappedMMIODevices;
    std::set<IODevice *> m_dynamicDevices;
};

}
