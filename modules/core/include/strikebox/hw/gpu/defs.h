// StrikeBox NV2A common definitions
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

namespace strikebox::nv2a {

// FIFO engines
// [https://envytools.readthedocs.io/en/latest/hw/fifo/intro.html#introduction]
enum class FIFOEngine : uint8_t { Software, PGRAPH };

// --- RAMHT -----------------------

union RAMHT {
    enum class Size : uint32_t { _4K, _8K, _16K, _32K };
    enum class Search : uint32_t { _16, _32, _64, _128 };

    uint32_t u32;
    struct {
        uint32_t
            : 4,              //  3.. 0 = unused
            baseAddress : 5,  //  8.. 5 = base address of hash table
            : 7;              // 15.. 9 = unused
        Size size : 2,        // 17..16 = size of hash table in bytes
            : 6;              // 23..18 = unused
        Search search : 2;    // 25..24 = entry search stride in bytes
    };

    // An entry in the hash table
    struct Entry {
        uint32_t handle;
        uint16_t instance;
        FIFOEngine engine : 2,
            : 6;
        uint8_t channelID : 5,
            : 2;
        bool valid : 1;
    };
    static_assert(sizeof(Entry) == 8);

    // Hash algorithm adapted from nouveau
    // [https://github.com/skeggsb/linux/blob/master/drivers/gpu/drm/nouveau/nvkm/core/ramht.c]
    inline uint32_t Hash(uint32_t handle, uint32_t channelID) {
        uint32_t bits = static_cast<uint32_t>(size) + 12;
        uint32_t hash = 0;

        while (handle) {
            hash ^= (handle & ((1 << bits) - 1));
            handle >>= bits;
        }

        return hash ^ channelID << (bits - 4);
    }
};
static_assert(sizeof(RAMHT) == sizeof(uint32_t));

// --- RAMFC -----------------------

union RAMFC {
    enum class Size : uint32_t { _1K, _2K };

    uint32_t u32;
    struct {
        uint32_t
            : 2,                    //  1.. 0 = unused
            baseAddress1 : 7,       //  8.. 2 = base address of FIFO context 1
            : 7;                    // 15.. 9 = unused
        Size size : 1;              // 16..16 = size of FIFO context in bytes
        uint32_t baseAddress2 : 7;  // 23..17 = base address of FIFO context 2
    };
};
static_assert(sizeof(RAMFC) == sizeof(uint32_t));

}
