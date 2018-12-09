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
inline uint32_t B2L24(uint8_t bytes[3]) {
    return (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];
}

// Converts a 24-bit unsigned integer split into low 16-bit and high 8-bit parts from big-endian to little-endian.
inline uint32_t B2L24S(uint8_t high, uint8_t low[2]) {
    return (high << 16) | B2L16(low);
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


// Converts a 16-bit unsigned integer from little-endian to big-endian.
inline void L2B16(uint8_t bytes[2], uint16_t val) {
    bytes[0] = val >> 8;
    bytes[1] = val;
}

// Converts a 24-bit unsigned integer from little-endian to big-endian.
inline void L2B24(uint8_t bytes[3], uint32_t val) {
    bytes[0] = val >> 16;
    bytes[1] = val >> 8;
    bytes[2] = val;
}

// Converts a 24-bit unsigned integer split into low 16-bit and high 8-bit parts from little-endian to big-endian.
inline void L2B24S(uint8_t *high, uint8_t low[2], uint32_t val) {
    *high = val >> 16;
    low[0] = val >> 8;
    low[1] = val;
}

// Converts a 32-bit unsigned integer from little-endian to big-endian.
inline void L2B32(uint8_t bytes[4], uint32_t val) {
    bytes[0] = val >> 24;
    bytes[1] = val >> 16;
    bytes[2] = val >> 8;
    bytes[3] = val;
}

// Converts a 64-bit unsigned integer from little-endian to big-endian.
inline void L2B64(uint8_t bytes[8], uint64_t val) {
    bytes[0] = val >> 56L;
    bytes[1] = val >> 48L;
    bytes[2] = val >> 40L;
    bytes[3] = val >> 32L;
    bytes[4] = val >> 24L;
    bytes[5] = val >> 16L;
    bytes[6] = val >> 8L;
    bytes[7] = val;
}

}
}
}
