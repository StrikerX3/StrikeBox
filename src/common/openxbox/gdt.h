#pragma once

#include <stdint.h>

namespace openxbox {

typedef struct _GDTEntry {
	union {
		struct {
			uint16_t limitLow : 16;
			uint16_t baseLow : 16;
			uint8_t baseMid : 8;
			uint8_t access : 8;
			uint8_t limitHigh : 4;
			uint8_t flags : 4;
			uint8_t baseHigh : 8;
		} data;
		uint64_t descriptor;
	};

	void Set(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
	
	uint32_t GetBase() {
		return ((data.baseLow) | (data.baseMid << 16) | (data.baseHigh << 24));
	}

	uint32_t GetLimit() {
		uint32_t limit = ((data.limitLow) | (data.limitHigh << 16));
		// If we use 4 KB pages, extend the limit to reflect that
		if (data.flags & 0x8) {
			limit = (limit << 12) | 0xfff;
		}
		return limit;
	}

} GDTEntry;

}
