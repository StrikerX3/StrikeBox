#pragma once

#include <stdint.h>

namespace openxbox {

const uint8_t kIDTG_Null00 = 0x0;
const uint8_t kIDTG_Task32 = 0x5;
const uint8_t kIDTG_Intr32 = 0xE;
const uint8_t kIDTG_Trap32 = 0xF;

typedef struct _IDTEntry {
	union {
		struct {
			uint16_t offsetLow : 16;
			uint16_t selector: 16;
			uint8_t zero : 8;
			uint8_t type : 4;
			uint8_t storageSegment : 1;
			uint8_t privilegeLevel : 2;
			uint8_t present : 1;
			uint16_t offsetHigh : 16;
		} data;
		uint64_t descriptor;
	};

	void Set(uint32_t offset, uint16_t selector, uint8_t type, uint8_t attributes);
	uint32_t GetOffset();
	void SetOffset(uint32_t offset);
} IDTEntry;

}
