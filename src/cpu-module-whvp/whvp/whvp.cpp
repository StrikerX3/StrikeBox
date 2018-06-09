#include "whvp.h"

#pragma comment(lib, "WinHvPlatform.lib")
#pragma comment(lib, "WinHvEmulation.lib")

WinHvPlatform::WinHvPlatform() {
    WHV_CAPABILITY cap;

    // Check for presence of the hypervisor platform
    WHvStatus status = GetCapability(WHvCapabilityCodeHypervisorPresent, &cap);
    if (WHVS_SUCCESS != status) {
        m_present = false;
        return;
    }

    m_present = cap.HypervisorPresent;
}

WinHvPlatform::~WinHvPlatform() {
    // Delete all partitions created with this object
    for (auto it = m_partitions.begin(); it != m_partitions.end(); it++) {
        delete (*it);
    }
    m_partitions.clear();
}

WHvStatus WinHvPlatform::GetCapability(WHV_CAPABILITY_CODE code, WHV_CAPABILITY *pCap) {
    UINT32 size;
    HRESULT hr = WHvGetCapability(code, pCap, sizeof(WHV_CAPABILITY), &size);
    if (S_OK != hr) {
        switch (hr) {
        case WHV_E_UNKNOWN_CAPABILITY:
            return WHVS_INVALID_CAPABILITY;
        default:
            return WHVS_FAILED;
        }
    }
    return WHVS_SUCCESS;
}

WHvPartitionStatus WinHvPlatform::CreatePartition(WHvPartition **ppPartition) {
    // Create and initialize the partition
    WHvPartition *partition = new WHvPartition(this);
    WHvPartitionStatus status = partition->Initialize();
    if (status != WHVPS_SUCCESS) {
        delete partition;
        return status;
    }

    // Add it to the vector so that we can clean up later
    m_partitions.push_back(partition);
    *ppPartition = partition;

    return WHVPS_SUCCESS;
}

WHvPartitionStatus WinHvPlatform::DeletePartition(WHvPartition **ppPartition) {
    // Null check the pointers
    if (ppPartition == nullptr) {
        return WHVPS_INVALID_POINTER;
    }
    if (*ppPartition == nullptr) {
        return WHVPS_INVALID_POINTER;
    }

    // Make sure the partition was created by this platform object
    if ((*ppPartition)->m_platform != this) {
        return WHVPS_INVALID_OWNER;
    }

    // Try to close the partition
    WHvPartitionStatus closeStatus = (*ppPartition)->Close();
    if (closeStatus != WHVPS_SUCCESS) {
        return closeStatus;
    }

    // Remove it from the clean up vector
    for (auto it = m_partitions.begin(); it != m_partitions.end(); it++) {
        if (*it == *ppPartition) {
            m_partitions.erase(it);
            break;
        }
    }
    
    // Delete and clear the pointer
    delete *ppPartition;
    *ppPartition = nullptr;
    
    return WHVPS_SUCCESS;
}


WHvPartition::WHvPartition(WinHvPlatform *platform)
    : m_platform(platform)
    , m_handle(INVALID_HANDLE_VALUE)
{
}

WHvPartition::~WHvPartition() {
    // Delete all VCPUs created with this partition
    for (auto it = m_vcpus.begin(); it != m_vcpus.end(); it++) {
        delete (*it);
    }
    m_vcpus.clear();

    // Release resources bound to this partition
    Close();
}

WHvPartitionStatus WHvPartition::Close() {
    // Check if the handle is valid
    if (m_handle == INVALID_HANDLE_VALUE) {
        return WHVPS_ALREADY_DELETED;
    }

    // Delete the partition
    HRESULT hr = WHvDeletePartition(m_handle);
    if (S_OK != hr) {
        return WHVPS_DELETE_FAILED;
    }
    
    // Clear the handle
    m_handle = INVALID_HANDLE_VALUE;

    return WHVPS_SUCCESS;
}

WHvPartitionStatus WHvPartition::Initialize() {
    // Check if the handle is valid
    if (m_handle != INVALID_HANDLE_VALUE) {
        return WHVPS_ALREADY_CREATED;
    }

    // Create the partition
    HRESULT hr = WHvCreatePartition(&m_handle);
    if (S_OK != hr) {
        m_handle = INVALID_HANDLE_VALUE;
        return WHVPS_CREATE_FAILED;
    }

    return WHVPS_SUCCESS;
}

WHvPartitionStatus WHvPartition::GetProperty(WHV_PARTITION_PROPERTY_CODE code, WHV_PARTITION_PROPERTY *ppProperty) {
    // Check if the handle is valid
    if (m_handle == INVALID_HANDLE_VALUE) {
        return WHVPS_UNINITIALIZED;
    }

    // Get the specified partition property
    UINT32 size;
    HRESULT hr = WHvGetPartitionProperty(m_handle, code, ppProperty, sizeof(WHV_PARTITION_PROPERTY), &size);
    if (S_OK != hr) {
        return WHVPS_FAILED;
    }
    return WHVPS_SUCCESS;
}

WHvPartitionStatus WHvPartition::SetProperty(WHV_PARTITION_PROPERTY_CODE code, WHV_PARTITION_PROPERTY *ppProperty) {
    // Check if the handle is valid
    if (m_handle == INVALID_HANDLE_VALUE) {
        return WHVPS_UNINITIALIZED;
    }

    // Set the specified partition property
    HRESULT hr = WHvSetPartitionProperty(m_handle, code, ppProperty, sizeof(WHV_PARTITION_PROPERTY));
    if (S_OK != hr) {
        return WHVPS_FAILED;
    }
    return WHVPS_SUCCESS;
}

WHvPartitionStatus WHvPartition::Setup() {
    // Check if the handle is valid
    if (m_handle == INVALID_HANDLE_VALUE) {
        return WHVPS_UNINITIALIZED;
    }

    // Setup the partition
    HRESULT hr = WHvSetupPartition(m_handle);
    if (S_OK != hr) {
        return WHVPS_SETUP_FAILED;
    }

    return WHVPS_SUCCESS;
}

WHvPartitionStatus WHvPartition::MapGpaRange(void *memory, WHV_GUEST_PHYSICAL_ADDRESS address, UINT64 size, WHV_MAP_GPA_RANGE_FLAGS flags) {
    // Check if the handle is valid
    if (m_handle == INVALID_HANDLE_VALUE) {
        return WHVPS_UNINITIALIZED;
    }

    // Map the memory to the specified guest physical address range
    HRESULT hr = WHvMapGpaRange(m_handle, memory, address, size, flags);
    if (S_OK != hr) {
        return WHVPS_FAILED;
    }

    return WHVPS_SUCCESS;
}

WHvPartitionStatus WHvPartition::UnmapGpaRange(WHV_GUEST_PHYSICAL_ADDRESS address, UINT64 size) {
    // Check if the handle is valid
    if (m_handle == INVALID_HANDLE_VALUE) {
        return WHVPS_UNINITIALIZED;
    }

    // Unmaps the specified guest physical address range
    HRESULT hr = WHvUnmapGpaRange(m_handle, address, size);
    if (S_OK != hr) {
        return WHVPS_FAILED;
    }

    return WHVPS_SUCCESS;
}

WHvVCPUStatus WHvPartition::CreateVCPU(WHvVCPU **ppVcpu, UINT32 vpIndex) {
    // Create and initialize the VCPU bound to this partition
    WHvVCPU *vcpu = new WHvVCPU(m_handle, vpIndex);
    WHvVCPUStatus status = vcpu->Initialize();
    if (status != WHVVCPUS_SUCCESS) {
        delete vcpu;
        return status;
    }

    // Add it to the vector so that we can clean up later
    m_vcpus.push_back(vcpu);
    *ppVcpu = vcpu;

    return WHVVCPUS_SUCCESS;
}

WHvVCPUStatus WHvPartition::DeleteVCPU(WHvVCPU **ppVcpu) {
    // Null check the pointers
    if (ppVcpu == nullptr) {
        return WHVVCPUS_INVALID_POINTER;
    }
    if (*ppVcpu == nullptr) {
        return WHVVCPUS_INVALID_POINTER;
    }

    // Make sure the VCPU was created by this partition
    if ((*ppVcpu)->m_partitionHandle != m_handle) {
        return WHVVCPUS_INVALID_OWNER;
    }

    // Try to close the VCPU
    WHvVCPUStatus closeStatus = (*ppVcpu)->Close();
    if (closeStatus != WHVPS_SUCCESS) {
        return closeStatus;
    }

    // Remove it from the clean up vector
    for (auto it = m_vcpus.begin(); it != m_vcpus.end(); it++) {
        if (*it == *ppVcpu) {
            m_vcpus.erase(it);
            break;
        }
    }

    // Delete and clear the pointer
    delete *ppVcpu;
    *ppVcpu = nullptr;

    return WHVVCPUS_SUCCESS;
}


WHvVCPU::WHvVCPU(WHV_PARTITION_HANDLE hPartition, UINT32 vpIndex)
    : m_partitionHandle(hPartition)
    , m_vpIndex(vpIndex)
    , m_initialized(false)
    , m_emuHandle(INVALID_HANDLE_VALUE)
    , m_ioPortCallback(nullptr)
    , m_memoryCallback(nullptr)
{
}

WHvVCPU::~WHvVCPU() {
    Close();
}

WHvVCPUStatus WHvVCPU::Close() {
    // Fail if the VCPU is not initialized
    if (!m_initialized) {
        return WHVVCPUS_NOT_INITIALIZED;
    }
    
    // Delete the VCPU
    HRESULT hr = WHvEmulatorDestroyEmulator(m_emuHandle);
    if (S_OK != hr) {
        return WHVVCPUS_DESTROY_EMU_FAILED;
    }

    // Mark as uninitialized
    m_initialized = false;

    return WHVVCPUS_SUCCESS;
}

WHvVCPUStatus WHvVCPU::Initialize() {
    // Fail if the VCPU is already initialized
    if (m_initialized) {
        return WHVVCPUS_ALREADY_INITIALIZED;
    }

    // Create the VCPU
    HRESULT hr = WHvCreateVirtualProcessor(m_partitionHandle, m_vpIndex, 0);
    if (S_OK != hr) {
        return WHVVCPUS_CREATE_FAILED;
    }
    
    WHV_EMULATOR_CALLBACKS callbacks;
    callbacks.Size = sizeof(WHV_EMULATOR_CALLBACKS);
    callbacks.Reserved = 0;
    callbacks.WHvEmulatorGetVirtualProcessorRegisters = GetVirtualProcessorRegistersCallback;
    callbacks.WHvEmulatorSetVirtualProcessorRegisters = SetVirtualProcessorRegistersCallback;
    callbacks.WHvEmulatorTranslateGvaPage = TranslateGvaPageCallback;
    callbacks.WHvEmulatorIoPortCallback = IoPortCallback;
    callbacks.WHvEmulatorMemoryCallback = MemoryCallback;
    hr = WHvEmulatorCreateEmulator(&callbacks, &m_emuHandle);
    if (S_OK != hr) {
        WHvDeleteVirtualProcessor(m_partitionHandle, m_vpIndex);
        return WHVVCPUS_CREATE_EMU_FAILED;
    }

    // Mark as initialized
    m_initialized = true;

    return WHVVCPUS_SUCCESS;
}

WHvVCPUStatus WHvVCPU::Run() {
    // Run the virtual processor
    HRESULT hr = WHvRunVirtualProcessor(m_partitionHandle, m_vpIndex, &m_exitContext, sizeof(m_exitContext));
    if (S_OK != hr) {
        return WHVVCPUS_RUN_FAILED;
    }

    switch (m_exitContext.ExitReason) {
    case WHvRunVpExitReasonX64IoPortAccess:
    {
        WHV_EMULATOR_STATUS emuStatus;
        hr = WHvEmulatorTryIoEmulation(m_emuHandle, this, &m_exitContext.VpContext, &m_exitContext.IoPortAccess, &emuStatus);
        if (S_OK != hr) {
            return WHVVCPUS_EMULATION_FAILED;
        }
        if (!emuStatus.EmulationSuccessful) {
            return WHVVCPUS_EMULATION_FAILED;
        }
        break;
    }
    case WHvRunVpExitReasonMemoryAccess:
    {
        WHV_EMULATOR_STATUS emuStatus;
        hr = WHvEmulatorTryMmioEmulation(m_emuHandle, this, &m_exitContext.VpContext, &m_exitContext.MemoryAccess, &emuStatus);
        if (S_OK != hr) {
            return WHVVCPUS_EMULATION_FAILED;
        }
        if (!emuStatus.EmulationSuccessful) {
            return WHVVCPUS_EMULATION_FAILED;
        }
        break;
    }
    }

    return WHVVCPUS_SUCCESS;
}

WHvVCPUStatus WHvVCPU::CancelRun() {
    // Cancel the virtual processor execution
    HRESULT hr = WHvCancelRunVirtualProcessor(m_partitionHandle, m_vpIndex, 0);
    if (S_OK != hr) {
        return WHVVCPUS_FAILED;
    }

    return WHVVCPUS_SUCCESS;
}

WHvVCPUStatus WHvVCPU::GetRegisters(const WHV_REGISTER_NAME *regs, UINT32 count, WHV_REGISTER_VALUE *values) {
    // Get specified registers
    HRESULT hr = WHvGetVirtualProcessorRegisters(m_partitionHandle, m_vpIndex, regs, count, values);
    if (S_OK != hr) {
        return WHVVCPUS_GET_REGS_FAILED;
    }

    return WHVVCPUS_SUCCESS;
}

WHvVCPUStatus WHvVCPU::SetRegisters(const WHV_REGISTER_NAME *regs, UINT32 count, const WHV_REGISTER_VALUE *values) {
    // Set specified registers
    HRESULT hr = WHvSetVirtualProcessorRegisters(m_partitionHandle, m_vpIndex, regs, count, values);
    if (S_OK != hr) {
        return WHVVCPUS_SET_REGS_FAILED;
    }

    return WHVVCPUS_SUCCESS;
}

WHvVCPUStatus WHvVCPU::Interrupt(uint16_t vector) {
    WHV_REGISTER_NAME regs[] = {
        WHvRegisterPendingInterruption,
    };
    WHV_REGISTER_VALUE vals[sizeof(regs) / sizeof(regs[0])];
    WHvVCPUStatus vcpuStatus = GetRegisters(regs, sizeof(regs) / sizeof(regs[0]), vals);
    if (WHVVCPUS_SUCCESS != vcpuStatus) {
        return vcpuStatus;
    }

    // Setup the interrupt
    vals[0].PendingInterruption.InterruptionPending = TRUE;
    vals[0].PendingInterruption.InterruptionType = WHvX64PendingInterrupt;
    vals[0].PendingInterruption.InterruptionVector = vector;

    vcpuStatus = SetRegisters(regs, sizeof(regs) / sizeof(regs[0]), vals);
    if (WHVVCPUS_SUCCESS != vcpuStatus) {
        return vcpuStatus;
    }

    return WHVVCPUS_SUCCESS;
}

HRESULT WHvVCPU::GetVirtualProcessorRegistersCallback(
    VOID* Context,
    const WHV_REGISTER_NAME* RegisterNames,
    UINT32 RegisterCount,
    WHV_REGISTER_VALUE* RegisterValues
) {
    WHvVCPU *vcpu = (WHvVCPU *) Context;
    return WHvGetVirtualProcessorRegisters(vcpu->m_partitionHandle, vcpu->m_vpIndex, RegisterNames, RegisterCount, RegisterValues);
}

HRESULT WHvVCPU::SetVirtualProcessorRegistersCallback(
    VOID* Context,
    const WHV_REGISTER_NAME* RegisterNames,
    UINT32 RegisterCount,
    const WHV_REGISTER_VALUE* RegisterValues
) {
    WHvVCPU *vcpu = (WHvVCPU *) Context;
    return WHvSetVirtualProcessorRegisters(vcpu->m_partitionHandle, vcpu->m_vpIndex, RegisterNames, RegisterCount, RegisterValues);
}

HRESULT WHvVCPU::TranslateGvaPageCallback(VOID* Context, WHV_GUEST_VIRTUAL_ADDRESS Gva, WHV_TRANSLATE_GVA_FLAGS TranslateFlags, WHV_TRANSLATE_GVA_RESULT_CODE* TranslationResult, WHV_GUEST_PHYSICAL_ADDRESS* Gpa) {
    WHvVCPU *vcpu = (WHvVCPU *)Context;
    WHV_TRANSLATE_GVA_RESULT result;
    HRESULT hr = WHvTranslateGva(vcpu->m_partitionHandle, vcpu->m_vpIndex, Gva, TranslateFlags, &result, Gpa);
    if (S_OK == hr) {
        *TranslationResult = result.ResultCode;
    }
    return hr;
}

HRESULT WHvVCPU::IoPortCallback(VOID *Context, WHV_EMULATOR_IO_ACCESS_INFO *IoAccess) {
    WHvVCPU *vcpu = (WHvVCPU *)Context;
    if (vcpu->m_ioPortCallback == nullptr) {
        return E_NOTIMPL;
    }
    return vcpu->m_ioPortCallback(Context, IoAccess);
}

HRESULT WHvVCPU::MemoryCallback(VOID *Context, WHV_EMULATOR_MEMORY_ACCESS_INFO *MemoryAccess) {
    WHvVCPU *vcpu = (WHvVCPU *)Context;
    if (vcpu->m_memoryCallback == nullptr) {
        return E_NOTIMPL;
    }
    return vcpu->m_memoryCallback(Context, MemoryAccess);
}
