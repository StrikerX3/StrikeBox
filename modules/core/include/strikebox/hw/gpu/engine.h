// Abstract base class for NV2A engines
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools and nouveau:
// https://envytools.readthedocs.io/en/latest/index.html
// https://github.com/torvalds/linux/tree/master/drivers/gpu/drm/nouveau
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#pragma once

#include <cstdint>
#include <string>

#include "defs.h"

namespace strikebox::nv2a {

class NV2A;

// Abstract base class of all NV2A engines.
class NV2AEngine {
public:
    NV2AEngine(const std::string name, const uint32_t offset, const uint32_t length, NV2A& nv2a)
        : m_name(name)
        , m_offset(offset)
        , m_length(length)
        , m_offsetEnd(offset + length)
        , m_nv2a(nv2a)
    {}

    virtual void Reset() = 0;
    virtual uint32_t Read(const uint32_t addr) = 0;
    virtual void Write(const uint32_t addr, const uint32_t value) = 0;

    virtual uint32_t ReadUnaligned(const uint32_t addr, const uint8_t size);
    virtual void WriteUnaligned(const uint32_t addr, const uint32_t value, const uint8_t size);

    const std::string GetName() const noexcept { return m_name; }
    const uint32_t GetOffset() const noexcept { return m_offset; }
    const uint32_t GetLength() const noexcept { return m_length; }

    const bool Contains(uint32_t address) const noexcept { return address >= m_offset && address < m_offsetEnd; }

protected:
    NV2A& m_nv2a;

    const std::string m_name;
    const uint32_t m_offset;
    const uint32_t m_length;
    const uint32_t m_offsetEnd;  // m_offset + m_length, precomputed for speed
};

}
