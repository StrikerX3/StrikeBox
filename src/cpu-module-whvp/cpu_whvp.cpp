#include <assert.h>
#include "cpu_whvp.h"

#include "openxbox/log.h"

namespace openxbox {
namespace cpu {

WhvpCpu::WhvpCpu() {
    m_whvp = nullptr;
    m_partition = nullptr;
    m_vcpu = nullptr;
}

WhvpCpu::~WhvpCpu() {
    if (m_whvp != nullptr) {
        // Deleting this object will automatically delete the partition and VCPU
        delete m_whvp;
        m_whvp = nullptr;
    }
}

CPUInitStatus WhvpCpu::InitializeImpl() {
    if (m_whvp == nullptr) {
        m_whvp = new WinHvPlatform();

        if (!m_whvp->IsPresent()) {
            return CPUS_INIT_PLATFORM_INIT_FAILED;
        }

        auto partStatus = m_whvp->CreatePartition(&m_partition);
        if (partStatus != WHVPS_SUCCESS) {
            delete m_whvp;
            m_whvp = nullptr;
            return CPUS_INIT_CREATE_VM_FAILED;
        }

        // Give one processor to the partition
        WHV_PARTITION_PROPERTY partitionProperty;
        partitionProperty.ProcessorCount = 1;
        partStatus = m_partition->SetProperty(WHvPartitionPropertyCodeProcessorCount, &partitionProperty);
        if (WHVPS_SUCCESS != partStatus) {
            return CPUS_INIT_CREATE_VM_FAILED;
        }

        // Setup the partition
        partStatus = m_partition->Setup();
        if (WHVPS_SUCCESS != partStatus) {
            return CPUS_INIT_CREATE_VM_FAILED;
        }

        auto vcpuStatus = m_partition->CreateVCPU(&m_vcpu, 0);
        if (vcpuStatus != WHVVCPUS_SUCCESS) {
            delete m_whvp;
            m_whvp = nullptr;
            return CPUS_INIT_CREATE_CPU_FAILED;
        }

        m_vcpu->SetIoPortCallback(IoPortCallback);
        m_vcpu->SetMemoryCallback(MemoryCallback);

        // Windows Hypervisor Platform VCPUs are initialized to EIP = 0xFFFF0
        // which comes from the 16-bit initialization address FFFF:FFF0,
        // but we expect it to be 0xFFFFFFF0 instead, so let's set this here
        RegWrite(REG_EIP, 0xFFFFFFF0);
    }

    return CPUS_INIT_OK;
}

CPUStatus WhvpCpu::RunImpl() {
    // Run CPU
    auto status = m_vcpu->Run();

    // Check VM exit status
    if (status != WHVVCPUS_SUCCESS) {
        return CPUS_FAILED;
    }

    // Handle exit status using tunnel
    switch (m_vcpu->ExitContext()->ExitReason) {
    case WHvRunVpExitReasonX64Halt:                  m_exitInfo.reason = CPU_EXIT_HLT;       break;  // HLT instruction
    case WHvRunVpExitReasonX64IoPortAccess:          m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // I/O (in / out instructions)
    case WHvRunVpExitReasonMemoryAccess:             m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // MMIO
    case WHvRunVpExitReasonX64InterruptWindow:       m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // Interrupt window (never requested, should never happen)
    case WHvRunVpExitReasonCanceled:                 m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // Execution cancelled
    case WHvRunVpExitReasonNone:                     m_exitInfo.reason = CPU_EXIT_NORMAL;    break;  // VM exited for no reason
    case WHvRunVpExitReasonException:                m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // VCPU threw an unhandled exception (the kernel should handle them)
    case WHvRunVpExitReasonX64Cpuid:                 m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // CPUID instruction (not enabled, should never happen)
    case WHvRunVpExitReasonX64MsrAccess:             m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // MSR access (not enabled, should never happen)
    case WHvRunVpExitReasonUnsupportedFeature:       m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // Host CPU does not support a feature needed by the VM
    case WHvRunVpExitReasonInvalidVpRegisterValue:   m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // VCPU has an invalid register
    case WHvRunVpExitReasonUnrecoverableException:   m_exitInfo.reason = CPU_EXIT_ERROR;     break;  // Unrecoverable exception
    }

    return CPUS_OK;
}

InterruptResult WhvpCpu::InterruptImpl(uint8_t vector) {
    // Cancel execution of the VCPU to give the emulator a chance to inject the interrupt request
    m_vcpu->CancelRun();

    return INTR_SUCCESS;
}

CPUMemMapStatus WhvpCpu::MemMapSubregion(MemoryRegion *subregion) {
    switch (subregion->m_type) {
    case MEM_REGION_MMIO:
    {
        // All unmapped regions in a Windows Hypervisor Platform VM are MMIO, no need to allocate
        return CPUS_MMAP_OK;
    }
    case MEM_REGION_NONE:
    {
        // This should not happen
        assert(0);
        return CPUS_MMAP_INVALID_TYPE;
    }
    case MEM_REGION_RAM:
    case MEM_REGION_ROM:
    {
        // Region is either RAM or ROM, map it accordingly
        WHV_MAP_GPA_RANGE_FLAGS flags = WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagExecute | ((subregion->m_type == MEM_REGION_RAM) ? WHvMapGpaRangeFlagWrite : WHvMapGpaRangeFlagNone);
        auto status = m_partition->MapGpaRange(subregion->m_data, subregion->m_start, subregion->m_size, flags);
        switch (status) {
        case WHVPS_SUCCESS: return CPUS_MMAP_OK;
        case WHVPS_UNINITIALIZED: return CPUS_MMAP_CPU_UNINITIALIZED;
        case WHVPS_FAILED: return CPUS_MMAP_MAPPING_FAILED;
        default: return CPUS_MMAP_UNHANDLED_ERROR;
        }
    }
    default:
        // This should not happen
        assert(0);
        return CPUS_MMAP_INVALID_TYPE;
    }
}

CPUOperationStatus WhvpCpu::RegRead(enum CpuReg reg, uint32_t *value) {
    WHV_REGISTER_NAME regs[1];
    WHV_REGISTER_VALUE vals[1];

    switch (reg) {
    case REG_EIP:    regs[0] = WHvX64RegisterRip;    break;
    case REG_EFLAGS: regs[0] = WHvX64RegisterRflags; break;
    case REG_EAX:    regs[0] = WHvX64RegisterRax;    break;
    case REG_ECX:    regs[0] = WHvX64RegisterRcx;    break;
    case REG_EDX:    regs[0] = WHvX64RegisterRdx;    break;
    case REG_EBX:    regs[0] = WHvX64RegisterRbx;    break;
    case REG_ESI:    regs[0] = WHvX64RegisterRsi;    break;
    case REG_EDI:    regs[0] = WHvX64RegisterRdi;    break;
    case REG_ESP:    regs[0] = WHvX64RegisterRsp;    break;
    case REG_EBP:    regs[0] = WHvX64RegisterRbp;    break;
    case REG_CS:     regs[0] = WHvX64RegisterCs;     break;
    case REG_SS:     regs[0] = WHvX64RegisterSs;     break;
    case REG_DS:     regs[0] = WHvX64RegisterDs;     break;
    case REG_ES:     regs[0] = WHvX64RegisterEs;     break;
    case REG_FS:     regs[0] = WHvX64RegisterFs;     break;
    case REG_GS:     regs[0] = WHvX64RegisterGs;     break;
    case REG_TR:     regs[0] = WHvX64RegisterTr;     break;
    case REG_CR0:    regs[0] = WHvX64RegisterCr0;    break;
    case REG_CR2:    regs[0] = WHvX64RegisterCr2;    break;
    case REG_CR3:    regs[0] = WHvX64RegisterCr3;    break;
    case REG_CR4:    regs[0] = WHvX64RegisterCr4;    break;
    default:                                         return CPUS_OP_INVALID_REGISTER;
    }

    auto status = m_vcpu->GetRegisters(regs, 1, vals);
    if (status != WHVVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }

    switch (reg) {
    case REG_EIP: case REG_EFLAGS: case REG_EAX: case REG_ECX: case REG_EDX: case REG_EBX:
    case REG_ESI: case REG_EDI: case REG_ESP: case REG_EBP: case REG_CR0: case REG_CR2: case REG_CR3: case REG_CR4:
        *value = vals[0].Reg32;
        break;
    case REG_CS: case REG_SS: case REG_DS: case REG_ES: case REG_FS: case REG_GS: case REG_TR:
        *value = vals[0].Segment.Selector;
        break;
    default:
        return CPUS_OP_INVALID_REGISTER;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus WhvpCpu::RegWrite(enum CpuReg reg, uint32_t value) {
    WHV_REGISTER_NAME regs[1];
    WHV_REGISTER_VALUE vals[1];

    switch (reg) {
    case REG_EIP:    regs[0] = WHvX64RegisterRip;    break;
    case REG_EFLAGS: regs[0] = WHvX64RegisterRflags; break;
    case REG_EAX:    regs[0] = WHvX64RegisterRax;    break;
    case REG_ECX:    regs[0] = WHvX64RegisterRcx;    break;
    case REG_EDX:    regs[0] = WHvX64RegisterRdx;    break;
    case REG_EBX:    regs[0] = WHvX64RegisterRbx;    break;
    case REG_ESI:    regs[0] = WHvX64RegisterRsi;    break;
    case REG_EDI:    regs[0] = WHvX64RegisterRdi;    break;
    case REG_ESP:    regs[0] = WHvX64RegisterRsp;    break;
    case REG_EBP:    regs[0] = WHvX64RegisterRbp;    break;
    case REG_CS:     regs[0] = WHvX64RegisterCs;     break;
    case REG_SS:     regs[0] = WHvX64RegisterSs;     break;
    case REG_DS:     regs[0] = WHvX64RegisterDs;     break;
    case REG_ES:     regs[0] = WHvX64RegisterEs;     break;
    case REG_FS:     regs[0] = WHvX64RegisterFs;     break;
    case REG_GS:     regs[0] = WHvX64RegisterGs;     break;
    case REG_TR:     regs[0] = WHvX64RegisterTr;     break;
    case REG_CR0:    regs[0] = WHvX64RegisterCr0;    break;
    case REG_CR2:    regs[0] = WHvX64RegisterCr2;    break;
    case REG_CR3:    regs[0] = WHvX64RegisterCr3;    break;
    case REG_CR4:    regs[0] = WHvX64RegisterCr4;    break;
    default:                                         return CPUS_OP_INVALID_REGISTER;
    }

    switch (reg) {
    case REG_EIP: case REG_EFLAGS: case REG_EAX: case REG_ECX: case REG_EDX: case REG_EBX:
    case REG_ESI: case REG_EDI: case REG_ESP: case REG_EBP: case REG_CR0: case REG_CR2: case REG_CR3: case REG_CR4:
        vals[0].Reg32 = value;
        break;
    case REG_CS: case REG_SS: case REG_DS: case REG_ES: case REG_FS: case REG_GS: case REG_TR:
    {
        vals[0].Segment.Selector = value;

        // Get GDT entry from memory
        uint32_t gdtAddr;
        uint32_t gdtSize;
        GetGDT(&gdtAddr, &gdtSize);

        GDTEntry gdtEntry;
        VMemRead(gdtAddr + value, sizeof(GDTEntry), &gdtEntry);

        // Fill in the rest of the CS info with data from the GDT entry
        vals[0].Segment.Attributes = gdtEntry.data.access | (gdtEntry.data.flags << 12);
        vals[0].Segment.Base = gdtEntry.GetBase();
        vals[0].Segment.Limit = gdtEntry.GetLimit();
        break;
    }
    default:
        return CPUS_OP_INVALID_REGISTER;
    }

    auto status = m_vcpu->SetRegisters(regs, 1, vals);
    if (status != WHVVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus WhvpCpu::RegRead(CpuReg regs[], uint32_t values[], uint8_t numRegs) {
    WHV_REGISTER_NAME *whvpRegs = new WHV_REGISTER_NAME[numRegs];
    WHV_REGISTER_VALUE *whvpVals = new WHV_REGISTER_VALUE[numRegs];

    for (uint8_t i = 0; i < numRegs; i++) {
        switch (regs[i]) {
        case REG_EIP:    whvpRegs[i] = WHvX64RegisterRip;    break;
        case REG_EFLAGS: whvpRegs[i] = WHvX64RegisterRflags; break;
        case REG_EAX:    whvpRegs[i] = WHvX64RegisterRax;    break;
        case REG_ECX:    whvpRegs[i] = WHvX64RegisterRcx;    break;
        case REG_EDX:    whvpRegs[i] = WHvX64RegisterRdx;    break;
        case REG_EBX:    whvpRegs[i] = WHvX64RegisterRbx;    break;
        case REG_ESI:    whvpRegs[i] = WHvX64RegisterRsi;    break;
        case REG_EDI:    whvpRegs[i] = WHvX64RegisterRdi;    break;
        case REG_ESP:    whvpRegs[i] = WHvX64RegisterRsp;    break;
        case REG_EBP:    whvpRegs[i] = WHvX64RegisterRbp;    break;
        case REG_CS:     whvpRegs[i] = WHvX64RegisterCs;     break;
        case REG_SS:     whvpRegs[i] = WHvX64RegisterSs;     break;
        case REG_DS:     whvpRegs[i] = WHvX64RegisterDs;     break;
        case REG_ES:     whvpRegs[i] = WHvX64RegisterEs;     break;
        case REG_FS:     whvpRegs[i] = WHvX64RegisterFs;     break;
        case REG_GS:     whvpRegs[i] = WHvX64RegisterGs;     break;
        case REG_TR:     whvpRegs[i] = WHvX64RegisterTr;     break;
        case REG_CR0:    whvpRegs[i] = WHvX64RegisterCr0;    break;
        case REG_CR2:    whvpRegs[i] = WHvX64RegisterCr2;    break;
        case REG_CR3:    whvpRegs[i] = WHvX64RegisterCr3;    break;
        case REG_CR4:    whvpRegs[i] = WHvX64RegisterCr4;    break;
        default: delete[] whvpRegs; delete[] whvpVals;       return CPUS_OP_INVALID_REGISTER;
        }
    }

    auto status = m_vcpu->GetRegisters(whvpRegs, numRegs, whvpVals);
    if (status != WHVVCPUS_SUCCESS) {
        delete[] whvpRegs;
        delete[] whvpVals;
        return CPUS_OP_FAILED;
    }

    for (uint8_t i = 0; i < numRegs; i++) {
        switch (regs[i]) {
        case REG_EIP: case REG_EFLAGS: case REG_EAX: case REG_ECX: case REG_EDX: case REG_EBX:
        case REG_ESI: case REG_EDI: case REG_ESP: case REG_EBP: case REG_CR0: case REG_CR2: case REG_CR3: case REG_CR4:
            values[i] = whvpVals[i].Reg32;
            break;
        case REG_CS: case REG_SS: case REG_DS: case REG_ES: case REG_FS: case REG_GS: case REG_TR:
            values[i] = whvpVals[i].Segment.Selector;
            break;
        default:
            delete[] whvpRegs;
            delete[] whvpVals;
            return CPUS_OP_INVALID_REGISTER;
        }
    }

    delete[] whvpRegs;
    delete[] whvpVals;
    return CPUS_OP_OK;
}

CPUOperationStatus WhvpCpu::RegWrite(CpuReg regs[], uint32_t values[], uint8_t numRegs) {
    WHV_REGISTER_NAME *whvpRegs = new WHV_REGISTER_NAME[numRegs];
    WHV_REGISTER_VALUE *whvpVals = new WHV_REGISTER_VALUE[numRegs];

    for (uint8_t i = 0; i < numRegs; i++) {
        switch (regs[i]) {
        case REG_EIP:    whvpRegs[i] = WHvX64RegisterRip;    break;
        case REG_EFLAGS: whvpRegs[i] = WHvX64RegisterRflags; break;
        case REG_EAX:    whvpRegs[i] = WHvX64RegisterRax;    break;
        case REG_ECX:    whvpRegs[i] = WHvX64RegisterRcx;    break;
        case REG_EDX:    whvpRegs[i] = WHvX64RegisterRdx;    break;
        case REG_EBX:    whvpRegs[i] = WHvX64RegisterRbx;    break;
        case REG_ESI:    whvpRegs[i] = WHvX64RegisterRsi;    break;
        case REG_EDI:    whvpRegs[i] = WHvX64RegisterRdi;    break;
        case REG_ESP:    whvpRegs[i] = WHvX64RegisterRsp;    break;
        case REG_EBP:    whvpRegs[i] = WHvX64RegisterRbp;    break;
        case REG_CS:     whvpRegs[i] = WHvX64RegisterCs;     break;
        case REG_SS:     whvpRegs[i] = WHvX64RegisterSs;     break;
        case REG_DS:     whvpRegs[i] = WHvX64RegisterDs;     break;
        case REG_ES:     whvpRegs[i] = WHvX64RegisterEs;     break;
        case REG_FS:     whvpRegs[i] = WHvX64RegisterFs;     break;
        case REG_GS:     whvpRegs[i] = WHvX64RegisterGs;     break;
        case REG_TR:     whvpRegs[i] = WHvX64RegisterTr;     break;
        case REG_CR0:    whvpRegs[i] = WHvX64RegisterCr0;    break;
        case REG_CR2:    whvpRegs[i] = WHvX64RegisterCr2;    break;
        case REG_CR3:    whvpRegs[i] = WHvX64RegisterCr3;    break;
        case REG_CR4:    whvpRegs[i] = WHvX64RegisterCr4;    break;
        default: delete[] whvpRegs; delete[] whvpVals;       return CPUS_OP_INVALID_REGISTER;
        }

        switch (regs[i]) {
        case REG_EIP: case REG_EFLAGS: case REG_EAX: case REG_ECX: case REG_EDX: case REG_EBX:
        case REG_ESI: case REG_EDI: case REG_ESP: case REG_EBP: case REG_CR0: case REG_CR2: case REG_CR3: case REG_CR4:
            whvpVals[i].Reg32 = values[i];
            break;
        case REG_CS: case REG_SS: case REG_DS: case REG_ES: case REG_FS: case REG_GS: case REG_TR:
        {
            whvpVals[i].Segment.Selector = values[i];

            // Get GDT entry from memory
            uint32_t gdtAddr;
            uint32_t gdtSize;
            GetGDT(&gdtAddr, &gdtSize);

            GDTEntry gdtEntry;
            VMemRead(gdtAddr + values[i], sizeof(GDTEntry), &gdtEntry);

            // Fill in the rest of the CS info with data from the GDT entry
            whvpVals[i].Segment.Attributes = gdtEntry.data.access | (gdtEntry.data.flags << 12);
            whvpVals[i].Segment.Base = gdtEntry.GetBase();
            whvpVals[i].Segment.Limit = gdtEntry.GetLimit();
            break;
        }
        default:
            delete[] whvpRegs;
            delete[] whvpVals;
            return CPUS_OP_INVALID_REGISTER;
        }
    }

    auto status = m_vcpu->SetRegisters(whvpRegs, numRegs, whvpVals);
    if (status != WHVVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus WhvpCpu::GetGDT(uint32_t *addr, uint32_t *size) {
    WHV_REGISTER_NAME reg[1] = { WHvX64RegisterGdtr };
    WHV_REGISTER_VALUE val[1];
    auto status = m_vcpu->GetRegisters(reg, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }
    *addr = val[0].Table.Base;
    *size = val[0].Table.Limit;

    return CPUS_OP_OK;
}

CPUOperationStatus WhvpCpu::SetGDT(uint32_t addr, uint32_t size) {
    WHV_REGISTER_NAME reg[1] = { WHvX64RegisterGdtr };
    WHV_REGISTER_VALUE val[1];
    val[0].Table.Base = addr;
    val[0].Table.Limit = size;

    auto status = m_vcpu->SetRegisters(reg, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus WhvpCpu::GetIDT(uint32_t *addr, uint32_t *size) {
    WHV_REGISTER_NAME reg[1] = { WHvX64RegisterIdtr };
    WHV_REGISTER_VALUE val[1];
    auto status = m_vcpu->GetRegisters(reg, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }
    *addr = val[0].Table.Base;
    *size = val[0].Table.Limit;

    return CPUS_OP_OK;
}

CPUOperationStatus WhvpCpu::SetIDT(uint32_t addr, uint32_t size) {
    WHV_REGISTER_NAME reg[1] = { WHvX64RegisterIdtr };
    WHV_REGISTER_VALUE val[1];
    val[0].Table.Base = addr;
    val[0].Table.Limit = size;

    auto status = m_vcpu->SetRegisters(reg, 1, val);
    if (status != WHVVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }

    return CPUS_OP_OK;
}

CPUOperationStatus WhvpCpu::InjectInterrupt(uint8_t vector) {
    auto status = m_vcpu->Interrupt(vector);
    if (status != WHVVCPUS_SUCCESS) {
        return CPUS_OP_FAILED;
    }

    return CPUS_OP_OK;
}

bool WhvpCpu::CanInjectInterrupt() {
    // TODO: should probably take a look at m_vcpu->ExitContext()->InterruptWindow
    return true;
}

void WhvpCpu::RequestInterruptWindow() {
    // TODO: should probably take a look at m_vcpu->ExitContext()->InterruptWindow
}

HRESULT WhvpCpu::IoPortCallback(PVOID context, WHV_EMULATOR_IO_ACCESS_INFO *io) {
    WhvpCpu *cpu = (WhvpCpu *)context;
    if (io->Direction == WHV_IO_OUT) {
        cpu->m_ioMapper->IOWrite(io->Port, io->Data, io->AccessSize);
    }
    else {
        cpu->m_ioMapper->IORead(io->Port, &io->Data, io->AccessSize);
    }
    return S_OK;
}

HRESULT WhvpCpu::MemoryCallback(PVOID context, WHV_EMULATOR_MEMORY_ACCESS_INFO *mem) {
    WhvpCpu *cpu = (WhvpCpu *)context;
    if (mem->Direction == WHV_IO_OUT) {
        cpu->m_ioMapper->MMIOWrite(mem->GpaAddress, *(uint32_t *)mem->Data, mem->AccessSize);
    }
    else {
        cpu->m_ioMapper->MMIORead(mem->GpaAddress, (uint32_t *)mem->Data, mem->AccessSize);
    }
    return S_OK;
}

}
}
