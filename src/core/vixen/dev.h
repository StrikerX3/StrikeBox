#pragma once

#include "vixen/memregion.h"

namespace vixen {

/*!
 * Device base class
 */
class Device {
protected:
public:
	Device(MemoryRegion *mem);
};

}
