#ifndef DEV_H
#define DEV_H

#include "mem.h"

/*!
 * Device base class
 */
class Device {
protected:
public:
    Device(MemoryRegion *mem);
};

#endif
