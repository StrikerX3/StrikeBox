// ATA/ATAPI-4 emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// Utility functions for dealing with ATAPI / SCSI data structures.
#pragma once

#include <cstdint>

namespace vixen {
namespace hw {
namespace atapi {

// Converts a 16-bit unsigned integer from big-endian to little-endian.
inline uint16_t B2L16(uint8_t bytes[2]) {
    return (bytes[0] << 8) | bytes[1];
}

// Converts a 24-bit unsigned integer from big-endian to little-endian.
inline uint32_t B2L24(uint8_t high, uint8_t low[2]) {
    return (high << 24) | B2L16(low);
}

// Converts a 32-bit unsigned integer from big-endian to little-endian.
inline uint32_t B2L32(uint8_t bytes[4]) {
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

// Converts a 64-bit unsigned integer from big-endian to little-endian.
inline uint64_t B2L64(uint8_t bytes[8]) {
    return ((uint64_t)bytes[0] << 56L) | ((uint64_t)bytes[1] << 48L) | ((uint64_t)bytes[2] << 40L) | ((uint64_t)bytes[3] << 32L)
        | ((uint64_t)bytes[4] << 24L) | ((uint64_t)bytes[5] << 16L) | ((uint64_t)bytes[6] << 8L) | (uint64_t)bytes[7];
}

}
}
}
