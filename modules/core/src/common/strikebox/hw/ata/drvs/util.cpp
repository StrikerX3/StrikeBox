#include "strikebox/hw/ata/drvs/util.h"

namespace strikebox {
namespace hw {
namespace ata {

void padString(uint8_t *dest, const char *src, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (*src) {
            dest[i ^ 1] = *src++;
        }
        else {
            dest[i ^ 1] = ' ';
        }
    }
}

}
}
}
