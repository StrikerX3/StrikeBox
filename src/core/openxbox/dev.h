#pragma once

#include "openxbox/memregion.h"

namespace openxbox {

/*!
 * Device base class
 */
class Device {
protected:
public:
	Device(MemoryRegion *mem);
};

}
