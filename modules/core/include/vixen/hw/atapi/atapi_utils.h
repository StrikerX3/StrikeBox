// ATAPI Command set emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// This code aims to implement the subset of the ATAPI Command set used by the
// Original Xbox to access the DVD drive.
//
// Based on:
//   [p] SCSI Primary Commands - 3 (SPC-3) Draft
//   http://t10.org/ftp/t10/document.08/08-309r1.pdf
//
//   [m] SCSI Multimedia Commands - 3 (MMC-3) Revision 10g
//   https://www.rockbox.org/wiki/pub/Main/DataSheets/mmc2r11a.pdf
//
//   [b] SCSI Block Commands - 3 (SBC-3) Revision 25
//   http://www.13thmonkey.org/documentation/SCSI/sbc3r25.pdf
//
//   [a] SCSI Architecture Model - 3 (SAM-3) Revision 13
//   http://www.csit-sun.pub.ro/~cpop/Documentatie_SMP/Standarde_magistrale/SCSI/sam3r13.pdf
//
//   [c] ATA Packet Interface for CD-ROMs Revision 2.6 Proposed
//   http://www.bswd.com/sff8020i.pdf
//
//   [s] SCSI Commands Reference Manual 100293068, Rev. J
//   https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
//
// References to particular items in the specification are denoted between brackets
// optionally followed by a quote from the specification. References are prefixed by
// the letter in brackets as listed above.
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
    bytes[0] = (uint8_t)(val >> 8);
    bytes[1] = (uint8_t)val;
}

// Converts a 24-bit unsigned integer from little-endian to big-endian.
inline void L2B24(uint8_t bytes[3], uint32_t val) {
    bytes[0] = (uint8_t)(val >> 16);
    bytes[1] = (uint8_t)(val >> 8);
    bytes[2] = (uint8_t)val;
}

// Converts a 24-bit unsigned integer split into low 16-bit and high 8-bit parts from little-endian to big-endian.
inline void L2B24S(uint8_t *high, uint8_t low[2], uint32_t val) {
    *high = (uint8_t)(val >> 16);
    low[0] = (uint8_t)(val >> 8);
    low[1] = (uint8_t)val;
}

// Converts a 32-bit unsigned integer from little-endian to big-endian.
inline void L2B32(uint8_t bytes[4], uint32_t val) {
    bytes[0] = (uint8_t)(val >> 24);
    bytes[1] = (uint8_t)(val >> 16);
    bytes[2] = (uint8_t)(val >> 8);
    bytes[3] = (uint8_t)val;
}

// Converts a 64-bit unsigned integer from little-endian to big-endian.
inline void L2B64(uint8_t bytes[8], uint64_t val) {
    bytes[0] = (uint8_t)(val >> 56L);
    bytes[1] = (uint8_t)(val >> 48L);
    bytes[2] = (uint8_t)(val >> 40L);
    bytes[3] = (uint8_t)(val >> 32L);
    bytes[4] = (uint8_t)(val >> 24L);
    bytes[5] = (uint8_t)(val >> 16L);
    bytes[6] = (uint8_t)(val >> 8L);
    bytes[7] = (uint8_t)val;
}

}
}
}
