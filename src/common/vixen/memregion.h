#pragma once

#include <stdint.h>
#include <string.h>

namespace vixen {

class MemoryRegion;

/*!
 * Memory Region Type enumeration
 */
enum MemoryRegionType
{
    MEM_REGION_NONE, // Region type is unspecified (may have multiple sub types)
    MEM_REGION_RAM,  // Region is RAM
	MEM_REGION_MMIO, // Region is Memory-Mapped IO
	MEM_REGION_ROM,  // Region is ROM
};

/*!
 * Memory Region Event Type enumeration
 */
enum MemoryRegionEventType
{
    MEM_EVENT_READ,
    MEM_EVENT_WRITE,
    MEM_EVENT_EXECUTE,
};

/*!
 * Memory Region Event
 */
struct MemoryRegionEvent {
    enum MemoryRegionEventType type;
    uint32_t                   addr;
    size_t                     size;
    uint64_t                   value;
};

/*!
 * Memory Region singly-linked list
 */
struct MemoryRegionLink {
	MemoryRegion     *curr;
	MemoryRegionLink *next;
};

/*!
 * Memory event handler function pointer prototype
 */
typedef int (*MemoryRegionEventHandler)(MemoryRegion *region, struct MemoryRegionEvent *event, void *user_data);

/*!
 * Generic memory region
 * 
 * This class is used to classify the various regions of memory. Subregions
 * can be added to regions for further classification.
 */
class MemoryRegion
{
public:
    MemoryRegion             *m_parent;
    MemoryRegionLink         *m_subregions;
    enum MemoryRegionType     m_type;
    uint32_t                  m_start;
    size_t                    m_size;
    void                     *m_data;
    MemoryRegionEventHandler  m_handler;
    void                     *m_handler_user;

    MemoryRegion(enum MemoryRegionType type, uint32_t start, size_t size, void *data);
	~MemoryRegion();
    uint32_t GetAbsoluteAddress(uint32_t offset);
    void SetParent(MemoryRegion *parent);
    void SetEventHandler(MemoryRegionEventHandler handler, void *user_data);
    int AddSubRegion(MemoryRegion *region);
};

}
