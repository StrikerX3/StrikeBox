#pragma once

#include <stdint.h>

namespace openxbox {

typedef struct Pte {
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
