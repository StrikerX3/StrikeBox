#pragma once

#include <cstdint>

namespace vixen {
namespace hw {
namespace ata {

void padString(uint8_t *dest, const char *src, uint32_t length);

}
}
}
