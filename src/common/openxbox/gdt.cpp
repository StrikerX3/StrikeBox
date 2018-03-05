#include "gdt.h"

namespace openxbox {

void _GDTEntry::Set(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
	this->descriptor = 0ULL;

	this->data.baseLow = base & 0xFFFF;
	this->data.baseMid = (base >> 16) & 0xFF;
	this->data.baseHigh = (base >> 24);

	this->data.limitLow = limit & 0xFFFF;
	this->data.limitHigh = (limit >> 16) & 0xF;
	
	this->data.access = access;
	this->data.flags = flags & 0xF;
}

}
