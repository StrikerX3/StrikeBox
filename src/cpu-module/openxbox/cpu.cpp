#include <assert.h>
#include <stdio.h>

#include "openxbox/cpu.h"
#include "openxbox/log.h"
#include "openxbox/pte.h"

namespace openxbox {

#define KB 1024
#define MB KB*KB
#define PAGE_SIZE 4*KB
#define PAGE_SIZE_LARGE 4*MB
#define PAGE_SHIFT 12

Cpu::Cpu() {
}

Cpu::~Cpu() {
}

// ----- Basic CPU operations -------------------------------------------------

int Cpu::Initialize() {
	for (uint8_t i = 0; i < 0x40; i++) {
		m_skippedInterrupts[i] = 0;
	}
	return InitializeImpl();
}

#define CHECK_RESULT(expr) result = (expr); { if (result) return result; }

int Cpu::Run(uint64_t time_limit_us) {
	return RunImpl(time_limit_us);
}

int Cpu::Step(uint64_t num_instructions) {
	return StepImpl(num_instructions);
}

InterruptResult Cpu::Interrupt(uint8_t vector) {
	return InterruptImpl(vector);
}

// ----- Physical memory ------------------------------------------------------

int Cpu::MemMap(MemoryRegion *mem) {
	// FIXME: We should flatten out the address space to handle sub-sub regions

	assert(mem->m_start == 0);

	for (auto it = mem->m_subregions; it != nullptr; it = it->next) {
		auto subregion = it->curr;
		log_debug("Mapping Region [%08x-%08zx)\n", subregion->m_start, subregion->m_start + subregion->m_size);
		MemMapSubregion(subregion);
	}
	return 0;
}

// ----- Virtual memory -------------------------------------------------------

bool Cpu::VirtualToPhysical(uint32_t vaddr, uint32_t *paddr) {
	// TODO: check MTRR

	// Get the PDE table address
	uint32_t cr3; // TODO: cache cr3 value
	RegRead(REG_CR3, &cr3);

	// Find the PDE entry corresponding to the given virtual address
	uint32_t pdeOffset = (vaddr >> 22) << 2;
	Pte pde;
	MemRead(cr3 + pdeOffset, sizeof(Pte), &pde);

	// If the PDE uses large pages, it points to a 4 MB page
	if (pde.largePage) {
		*paddr = (pde.pageFrameNumber << PAGE_SHIFT) | (vaddr & (PAGE_SIZE_LARGE - 1));
		return true;
	}

	// If the PDE is not valid, the virtual address is not valid
	if (!pde.valid) {
		return false;
	}

	// Get the page where the PTE table corresponding to the PDE is located
	uint32_t pteTableAddr = (pde.pageFrameNumber << PAGE_SHIFT);

	// Find the PTE entry
	uint32_t pteOffset = ((vaddr << 10) >> 22) << 2;
    Pte pte;
	MemRead(pteTableAddr + pteOffset, sizeof(Pte), &pte);

	// If the PTE is not valid, the virtual address is not valid
	if (!pte.valid) {
		return false;
	}

	// The physical address is located at the corresponding 4 KB page
	*paddr = (pte.pageFrameNumber << PAGE_SHIFT) | (vaddr & (PAGE_SIZE - 1));
	return true;
}

int Cpu::VMemRead(uint32_t vaddr, uint32_t size, void *value) {
	uint32_t srcAddrStart = vaddr;
	uint32_t srcAddrEnd = ((srcAddrStart + PAGE_SIZE) & ~(PAGE_SIZE - 1)) - 1;
	uint32_t pos = 0;
	uint32_t copySize = srcAddrEnd - srcAddrStart + 1;
	if (size < copySize) {
		copySize = size;
	}
	while (pos < size) {
		uint32_t physAddr;
		if (!VirtualToPhysical(srcAddrStart, &physAddr)) { return -1; }

		int result = MemRead(physAddr, copySize, &((char*)value)[pos]);
		if (result) { return result; }

		pos += copySize;
		srcAddrStart = srcAddrEnd + 1;
		srcAddrEnd += PAGE_SIZE;
		copySize = size - pos;
		if (PAGE_SIZE < copySize) {
			copySize = PAGE_SIZE;
		}
	}
	return 0;
}

int Cpu::VMemWrite(uint32_t vaddr, uint32_t size, void *value) {
	uint32_t srcAddrStart = vaddr;
	uint32_t srcAddrEnd = ((srcAddrStart + PAGE_SIZE) & ~(PAGE_SIZE - 1)) - 1;
	uint32_t pos = 0;
	uint32_t copySize = srcAddrEnd - srcAddrStart + 1;
	if (size < copySize) {
		copySize = size;
	}
	while (pos < size) {
		uint32_t physAddr;
		if (!VirtualToPhysical(srcAddrStart, &physAddr)) { return -1; }

		int result = MemWrite(physAddr, copySize, &((char*)value)[pos]);
		if (result) { return result; }

		pos += copySize;
		srcAddrStart = srcAddrEnd + 1;
		srcAddrEnd += PAGE_SIZE;
		copySize = size - pos;
		if (PAGE_SIZE < copySize) {
			copySize = PAGE_SIZE;
		}
	}
	return 0;
}

// ----- Stack ----------------------------------------------------------------

int Cpu::CreateStackSpace(uint32_t size) {
	int result;
	uint32_t esp;
	CHECK_RESULT(RegRead(REG_ESP, &esp));
	esp -= size;
	CHECK_RESULT(RegWrite(REG_ESP, esp));
	return 0;
}

int Cpu::ReclaimStackSpace(uint32_t size) {
	int result;
	uint32_t esp;
	CHECK_RESULT(RegRead(REG_ESP, &esp));
	esp += size;
	CHECK_RESULT(RegWrite(REG_ESP, esp));
	return 0;
}

// ----- Registers ------------------------------------------------------------

int Cpu::RegCopy(enum CpuReg dst, enum CpuReg src) {
	int result;
	uint32_t tmp;
	CHECK_RESULT(RegRead(src, &tmp));
	CHECK_RESULT(RegWrite(dst, tmp));
	return 0;
}

int Cpu::GetGDTEntry(uint16_t selector, GDTEntry *entry) {
	int result;
	uint32_t base;
	uint32_t limit;
	CHECK_RESULT(GetGDT(&base, &limit));
	if (selector + sizeof(GDTEntry) > limit) {
		return -1;
	}
	CHECK_RESULT(MemRead(base + selector, sizeof(GDTEntry), entry));
	return 0;
}

int Cpu::SetGDTEntry(uint16_t selector, GDTEntry *entry) {
	int result;
	uint32_t base;
	uint32_t limit;
	CHECK_RESULT(GetGDT(&base, &limit));
	if (selector + sizeof(GDTEntry) > limit) {
		return -1;
	}
	CHECK_RESULT(MemWrite(base + selector, sizeof(GDTEntry), entry));
	return 0;
}

int Cpu::GetIDTEntry(uint8_t vector, IDTEntry *entry) {
	int result;
	uint32_t base;
	uint32_t limit;
	CHECK_RESULT(GetIDT(&base, &limit));
	if (vector * sizeof(IDTEntry) > limit) {
		return -1;
	}
	CHECK_RESULT(MemRead(base + vector * sizeof(IDTEntry), sizeof(IDTEntry), entry));
	return 0;
}

int Cpu::SetIDTEntry(uint8_t vector, IDTEntry *entry) {
	int result;
	uint32_t base;
	uint32_t limit;
	CHECK_RESULT(GetIDT(&base, &limit));
	if (vector * sizeof(IDTEntry) > limit) {
		return -1;
	}
	CHECK_RESULT(MemWrite(base + vector * sizeof(IDTEntry), sizeof(IDTEntry), entry));
	return 0;
}

void Cpu::SetInterruptsEnabled(bool enabled) {
	uint32_t flags;
	RegRead(REG_EFLAGS, &flags);
	if (enabled) {
		flags |= IF_MASK;
	}
	else {
		flags &= ~IF_MASK;
	}
	RegWrite(REG_EFLAGS, flags);
}

int Cpu::SetFlags(uint32_t flagsBits) {
	int result;
	uint32_t flags;
	CHECK_RESULT(RegRead(REG_EFLAGS, &flags));
	flags |= flagsBits;
	CHECK_RESULT(RegWrite(REG_EFLAGS, flags));
	return 0;
}

int Cpu::ClearFlags(uint32_t flagsBits) {
	int result;
	uint32_t flags;
	CHECK_RESULT(RegRead(REG_EFLAGS, &flags));
	flags &= ~flagsBits;
	CHECK_RESULT(RegWrite(REG_EFLAGS, flags));
	return 0;
}

// ----- Instructions ---------------------------------------------------------

int Cpu::Push(uint32_t value) {
	int result;
	uint32_t esp;
	CHECK_RESULT(RegRead(REG_ESP, &esp));
	esp -= 4;
	CHECK_RESULT(VMemWrite(esp, 4, &value));
	CHECK_RESULT(RegWrite(REG_ESP, esp));
	return 0;
}

int Cpu::PushReg(enum CpuReg reg) {
	int result;
	uint32_t value;
	CHECK_RESULT(RegRead(reg, &value));
	CHECK_RESULT(Push(value));
	return 0;
}

int Cpu::PushFlags() {
	return PushReg(REG_EFLAGS);
}

int Cpu::Pop(uint32_t *value) {
	int result;
	uint32_t esp;
	CHECK_RESULT(RegRead(REG_ESP, &esp));
	CHECK_RESULT(VMemRead(esp, 4, value));
	esp += 4;
	CHECK_RESULT(RegWrite(REG_ESP, esp));
	return 0;
}

int Cpu::PopReg(enum CpuReg reg) {
	int result;
	uint32_t value;
	CHECK_RESULT(Pop(&value));
	CHECK_RESULT(RegWrite(reg, value));
	return 0;
}

int Cpu::PopFlags() {
	return PopReg(REG_EFLAGS);
}

int Cpu::Ret() {
	return PopReg(REG_EIP);
}

}
