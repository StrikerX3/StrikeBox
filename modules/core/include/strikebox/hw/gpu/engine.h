// Abstract base class for NV2A engines
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#pragma once

#include <cstdint>
#include <string>

#include "state.h"

namespace strikebox::nv2a {

// Abstract base class of all NV2A engines.
class NV2AEngine {
public:
    NV2AEngine(const std::string name, const uint32_t offset, const uint32_t length, const NV2A& nv2a)
        : m_name(name)
        , m_offset(offset)
        , m_length(length)
        , m_offsetEnd(offset + length)
        , m_nv2a(nv2a)
    {}

    virtual uint32_t Read(const uint32_t addr, const uint8_t size) = 0;
    virtual void Write(const uint32_t addr, const uint32_t value, const uint8_t size) = 0;

    const std::string GetName() const noexcept { return m_name; }
    const uint32_t GetOffset() const noexcept { return m_offset; }
    const uint32_t GetLength() const noexcept { return m_length; }

    const bool Contains(uint32_t address) const noexcept { return address >= m_offset && address < m_offsetEnd; }

private:
    const std::string m_name;
    const uint32_t m_offset;
    const uint32_t m_length;
    const uint32_t m_offsetEnd;  // m_offset + m_length, precomputed for speed
    const NV2A& m_nv2a;
};

}
