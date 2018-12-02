#include <assert.h>
#include <stdio.h>

#include "openxbox/cpu.h"
#include "openxbox/log.h"
#include "openxbox/pte.h"

namespace openxbox {
namespace cpu {

#define KB 1024
#define MB KB*KB
#define PAGE_SIZE 4*KB
#define PAGE_SIZE_LARGE 4*MB
#define PAGE_SHIFT 12

Cpu::Cpu() {
}

Cpu::~Cpu() {
    // Clear the physical memory map
    for (auto it = m_physMemMap.begin(); it != m_physMemMap.end(); it++) {
        delete *it;
    }
    m_physMemMap.clear();
}

// ----- Basic CPU operations -------------------------------------------------

CPUInitStatus Cpu::Initialize(IOMapper *ioMapper) {
    m_ioMapper = ioMapper;

    m_interruptHandlerCredits = kInterruptHandlerMaxCredits;

    return InitializeImpl();
}

#define CHECK_RESULT(expr) do { CPUOperationStatus result = (expr); { if (result != CPUS_OP_OK) return result; } } while (0)

CPUStatus Cpu::Run() {
    HandleInterruptQueue();
    return RunImpl();
}

CPUStatus Cpu::Step() {
    HandleInterruptQueue();
    return StepImpl();
}

CPUStatus Cpu::StepImpl() {
    return CPUS_UNSUPPORTED;
}

InterruptResult Cpu::Interrupt(uint8_t vector) {
    // Acquire the pending interrupts mutex
    std::lock_guard<std::mutex> guard(m_pendingInterruptsMutex);

    // Enqueue the interrupt
    m_pendingInterrupts.push(vector);

    return InterruptImpl(vector);
}

// ----- Physical memory ------------------------------------------------------

CPUMemMapStatus Cpu::MemMap(MemoryRegion *mem) {
    // FIXME: We should flatten out the address space to handle sub-sub regions

    assert(mem->m_start == 0);

    for (auto it = mem->m_subregions; it != nullptr; it = it->next) {
        auto subregion = it->curr;
        log_debug("Mapping Region %08x - %08zx\n", subregion->m_start, subregion->m_start + subregion->m_size - 1);
        CPUMemMapStatus status = MemMapSubregion(subregion);
        if (status != CPUS_MMAP_OK) {
            log_error("  Failed to map subregion\n");
            return status;
        }

        // Map the physical address range if valid
        if (subregion->m_type == MEM_REGION_RAM || subregion->m_type == MEM_REGION_ROM) {
            m_physMemMap.push_back(new PhysicalMemoryRange{ (char *)subregion->m_data, subregion->m_start, subregion->m_start + (uint32_t)subregion->m_size - 1 });
        }
    }
    return CPUS_MMAP_OK;
}

CPUOperationStatus Cpu::MemRead(uint32_t addr, uint32_t size, void *value) {
    for (auto it = m_physMemMap.begin(); it != m_physMemMap.end(); it++) {
        auto physMemRegion = *it;
        if (addr >= physMemRegion->startingAddress && addr + size - 1 <= physMemRegion->endingAddress) {
            memcpy(value, &physMemRegion->data[addr - physMemRegion->startingAddress], size);
            return CPUS_OP_OK;
        }
    }

    return CPUS_OP_INVALID_ADDRESS;
}

CPUOperationStatus Cpu::MemWrite(uint32_t addr, uint32_t size, void *value) {
    for (auto it = m_physMemMap.begin(); it != m_physMemMap.end(); it++) {
        auto physMemRegion = *it;
        if (addr >= physMemRegion->startingAddress && addr + size - 1 <= physMemRegion->endingAddress) {
            memcpy(&physMemRegion->data[addr - physMemRegion->startingAddress], value, size);
            return CPUS_OP_OK;
        }
    }

    return CPUS_OP_INVALID_ADDRESS;
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

CPUOperationStatus Cpu::VMemRead(uint32_t vaddr, uint32_t size, void *value, uint32_t *bytesRead) {
    uint32_t srcAddrStart = vaddr;
    uint32_t srcAddrEnd = ((srcAddrStart + PAGE_SIZE) & ~(PAGE_SIZE - 1)) - 1;
    uint32_t pos = 0;
    uint32_t copySize = srcAddrEnd - srcAddrStart + 1;
    if (size < copySize) {
        copySize = size;
    }
    while (pos < size) {
        uint32_t physAddr;
        if (!VirtualToPhysical(srcAddrStart, &physAddr)) {
            return CPUS_OP_INVALID_ADDRESS;
        }

        CPUOperationStatus result = MemRead(physAddr, copySize, &((char*)value)[pos]);
        if (result != CPUS_OP_OK) {
            return result;
        }

        pos += copySize;
        srcAddrStart = srcAddrEnd + 1;
        srcAddrEnd += PAGE_SIZE;
        copySize = size - pos;
        if (PAGE_SIZE < copySize) {
            copySize = PAGE_SIZE;
        }
    }

    if (bytesRead != nullptr) {
        *bytesRead = pos;
    }
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::VMemWrite(uint32_t vaddr, uint32_t size, void *value, uint32_t *bytesWritten) {
    uint32_t srcAddrStart = vaddr;
    uint32_t srcAddrEnd = ((srcAddrStart + PAGE_SIZE) & ~(PAGE_SIZE - 1)) - 1;
    uint32_t pos = 0;
    uint32_t copySize = srcAddrEnd - srcAddrStart + 1;
    if (size < copySize) {
        copySize = size;
    }
    while (pos < size) {
        uint32_t physAddr;
        if (!VirtualToPhysical(srcAddrStart, &physAddr)) {
            return CPUS_OP_INVALID_ADDRESS;
        }

        CPUOperationStatus result = MemWrite(physAddr, copySize, &((char*)value)[pos]);
        if (result != CPUS_OP_OK) {
            return result;
        }

        pos += copySize;
        srcAddrStart = srcAddrEnd + 1;
        srcAddrEnd += PAGE_SIZE;
        copySize = size - pos;
        if (PAGE_SIZE < copySize) {
            copySize = PAGE_SIZE;
        }
    }

    if (bytesWritten != nullptr) {
        *bytesWritten = pos;
    }
    return CPUS_OP_OK;
}

// ----- Stack ----------------------------------------------------------------

CPUOperationStatus Cpu::CreateStackSpace(uint32_t size) {
    uint32_t esp;
    CHECK_RESULT(RegRead(REG_ESP, &esp));
    esp -= size;
    CHECK_RESULT(RegWrite(REG_ESP, esp));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::ReclaimStackSpace(uint32_t size) {
    uint32_t esp;
    CHECK_RESULT(RegRead(REG_ESP, &esp));
    esp += size;
    CHECK_RESULT(RegWrite(REG_ESP, esp));
    return CPUS_OP_OK;
}

// ----- Registers ------------------------------------------------------------

CPUOperationStatus Cpu::RegCopy(enum CpuReg dst, enum CpuReg src) {
    uint32_t tmp;
    CHECK_RESULT(RegRead(src, &tmp));
    CHECK_RESULT(RegWrite(dst, tmp));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::RegRead(CpuReg regs[], uint32_t values[], uint8_t numRegs) {
    for (uint8_t i = 0; i < numRegs; i++) {
        CPUOperationStatus status = RegRead(regs[i], &values[i]);
        if (status != CPUS_OP_OK) {
            return status;
        }
    }
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::RegWrite(CpuReg regs[], uint32_t values[], uint8_t numRegs) {
    for (uint8_t i = 0; i < numRegs; i++) {
        CPUOperationStatus status = RegWrite(regs[i], values[i]);
        if (status != CPUS_OP_OK) {
            return status;
        }
    }
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::RegCopy(CpuReg dsts[], CpuReg srcs[], uint8_t numRegs) {
    for (uint8_t i = 0; i < numRegs; i++) {
        CPUOperationStatus status = RegCopy(dsts[i], srcs[i]);
        if (status != CPUS_OP_OK) {
            return status;
        }
    }
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::GetGDTEntry(uint16_t selector, GDTEntry *entry) {
    uint32_t base;
    uint32_t limit;
    CHECK_RESULT(GetGDT(&base, &limit));
    if (selector + sizeof(GDTEntry) > limit) {
        return CPUS_OP_INVALID_SELECTOR;
    }
    CHECK_RESULT(MemRead(base + selector, sizeof(GDTEntry), entry));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::SetGDTEntry(uint16_t selector, GDTEntry *entry) {
    uint32_t base;
    uint32_t limit;
    CHECK_RESULT(GetGDT(&base, &limit));
    if (selector + sizeof(GDTEntry) > limit) {
        return CPUS_OP_INVALID_SELECTOR;
    }
    CHECK_RESULT(MemWrite(base + selector, sizeof(GDTEntry), entry));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::GetIDTEntry(uint8_t vector, IDTEntry *entry) {
    uint32_t base;
    uint32_t limit;
    CHECK_RESULT(GetIDT(&base, &limit));
    if (vector * sizeof(IDTEntry) > limit) {
        return CPUS_OP_INVALID_SELECTOR;
    }
    CHECK_RESULT(MemRead(base + vector * sizeof(IDTEntry), sizeof(IDTEntry), entry));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::SetIDTEntry(uint8_t vector, IDTEntry *entry) {
    uint32_t base;
    uint32_t limit;
    CHECK_RESULT(GetIDT(&base, &limit));
    if (vector * sizeof(IDTEntry) > limit) {
        return CPUS_OP_INVALID_SELECTOR;
    }
    CHECK_RESULT(MemWrite(base + vector * sizeof(IDTEntry), sizeof(IDTEntry), entry));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::SetInterruptsEnabled(bool enabled) {
    uint32_t flags;
    CHECK_RESULT(RegRead(REG_EFLAGS, &flags));
    if (enabled) {
        flags |= IF_MASK;
    }
    else {
        flags &= ~IF_MASK;
    }
    CHECK_RESULT(RegWrite(REG_EFLAGS, flags));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::SetFlags(uint32_t flagsBits) {
    uint32_t flags;
    CHECK_RESULT(RegRead(REG_EFLAGS, &flags));
    flags |= flagsBits;
    CHECK_RESULT(RegWrite(REG_EFLAGS, flags));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::ClearFlags(uint32_t flagsBits) {
    uint32_t flags;
    CHECK_RESULT(RegRead(REG_EFLAGS, &flags));
    flags &= ~flagsBits;
    CHECK_RESULT(RegWrite(REG_EFLAGS, flags));
    return CPUS_OP_OK;
}

// ----- Instructions ---------------------------------------------------------

CPUOperationStatus Cpu::Push(uint32_t value) {
    uint32_t esp;
    CHECK_RESULT(RegRead(REG_ESP, &esp));
    esp -= 4;
    CHECK_RESULT(VMemWrite(esp, 4, &value));
    CHECK_RESULT(RegWrite(REG_ESP, esp));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::PushReg(enum CpuReg reg) {
    uint32_t value;
    CHECK_RESULT(RegRead(reg, &value));
    CHECK_RESULT(Push(value));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::PushFlags() {
    return PushReg(REG_EFLAGS);
}

CPUOperationStatus Cpu::Pop(uint32_t *value) {
    uint32_t esp;
    CHECK_RESULT(RegRead(REG_ESP, &esp));
    CHECK_RESULT(VMemRead(esp, 4, value));
    esp += 4;
    CHECK_RESULT(RegWrite(REG_ESP, esp));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::PopReg(enum CpuReg reg) {
    uint32_t value;
    CHECK_RESULT(Pop(&value));
    CHECK_RESULT(RegWrite(reg, value));
    return CPUS_OP_OK;
}

CPUOperationStatus Cpu::PopFlags() {
    return PopReg(REG_EFLAGS);
}

CPUOperationStatus Cpu::Ret() {
    return PopReg(REG_EIP);
}

CPUOperationStatus Cpu::EnableSoftwareBreakpoints(bool enable) {
    return CPUS_OP_UNSUPPORTED;
}

CPUOperationStatus Cpu::SetHardwareBreakpoints(HardwareBreakpoints breakpoints) {
    return CPUS_OP_UNSUPPORTED;
}

CPUOperationStatus Cpu::ClearHardwareBreakpoints() {
    return CPUS_OP_UNSUPPORTED;
}

CPUOperationStatus Cpu::GetBreakpointAddress(uint32_t *address) {
    return CPUS_OP_UNSUPPORTED;
}

void Cpu::HandleInterruptQueue() {
    // Increment the credits available for the interrupt handler
    if (m_interruptHandlerCredits < kInterruptHandlerMaxCredits) {
        m_interruptHandlerCredits += kInterruptHandlerIncrement;
    }

    // Inject an interrupt if available and possible
    if (m_pendingInterrupts.size() > 0) {
        if (CanInjectInterrupt()) {
            InjectPendingInterrupt();
        }
        // Request an interrupt window if the VCPU is not ready
        else {
            RequestInterruptWindow();
        }
    }
}

void Cpu::InjectPendingInterrupt() {
    // If there aren't enough credits or there are no pending interrupts, get out
    if (m_interruptHandlerCredits < kInterruptHandlerCost || m_pendingInterrupts.size() == 0) {
        return;
    }

    // Spend the credits and handle one interrupt
    m_interruptHandlerCredits -= kInterruptHandlerCost;

    // Acquire the pending interrupts mutex
    std::lock_guard<std::mutex> guard(m_pendingInterruptsMutex);

    // Dequeue the next interrupt vector
    uint8_t vector = m_pendingInterrupts.front();
    m_pendingInterrupts.pop();

#ifdef _DEBUG
    if (m_pendingInterrupts.size() > 100) {
        log_warning("Pending interrupts queue growing too long!\n");
    }
#endif

    // Inject the interrupt into the VCPU
    InjectInterrupt(vector);

    return;
}

}
}
