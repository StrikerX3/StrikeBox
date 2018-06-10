#pragma once

#include <stdint.h>

namespace openxbox {

#define ONE_KB 1024
#define ONE_MB (1024 * 1024)

#define XBOX_MEMORY_SIZE (64 * ONE_MB)

#define XBE_IMAGE_BASE 0x00010000
#define CONTIGUOUS_MEMORY_BASE  0x80000000

struct Pte {
    uint32_t valid : 1;
    uint32_t write : 1;
    uint32_t owner : 1;
    uint32_t writeThrough : 1;
    uint32_t cacheDisable : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t largePage : 1;
    uint32_t global : 1;
    uint32_t guardOrEndOfAllocation : 1;
    uint32_t persistAllocation : 1;
    uint32_t _reserved : 1;
    uint32_t pageFrameNumber : 20;
};

}
