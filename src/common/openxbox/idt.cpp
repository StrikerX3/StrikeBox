#include "idt.h"

namespace openxbox {

void _IDTEntry::Set(uint32_t offset, uint16_t selector, uint8_t type, uint8_t attributes) {
	this->descriptor = 0ULL;

	this->data.offsetLow = offset & 0xFFFF;
	this->data.offsetHigh = (offset >> 16);

	this->data.selector = selector;
	
	this->data.type = type & 0xF;
	this->data.storageSegment = attributes & 0x1;
	this->data.privilegeLevel = (attributes >> 1) & 0x3;
	this->data.present = (attributes >> 3) & 0x1;
}

uint32_t _IDTEntry::GetOffset() {
	return ((uint32_t)data.offsetLow) | (((uint32_t)data.offsetHigh) << 16);
}

void _IDTEntry::SetOffset(uint32_t offset) {
	this->data.offsetLow = offset & 0xFFFF;
	this->data.offsetHigh = (offset >> 16);
}

}
