#pragma once

#include <Windows.h>
#include <WinHvPlatform.h>
#include <WinHvEmulation.h>

#include <vector>

enum WHvStatus {
    WHVS_SUCCESS = 0,                    // The operation completed successfully

    WHVS_FAILED = 0x80000000,            // The operation failed
    WHVS_INVALID_CAPABILITY,             // An invalid capability code was passed
};

enum WHvPartitionStatus {
    WHVPS_SUCCESS = 0,                   // Partition created successfully

    WHVPS_FAILED = 0x80000000,           // The operation failed
    WHVPS_CREATE_FAILED,                 // Failed to create partition
    WHVPS_DELETE_FAILED,                 // Failed to delete a partition
    WHVPS_SETUP_FAILED,                  // Failed to setup a partition
    WHVPS_ALREADY_CREATED,               // Attempted to create a partition that was already created
    WHVPS_ALREADY_DELETED,               // Attempted to delete a partition that was already deleted
    WHVPS_INVALID_OWNER,                 // Attempted to delete a partition that does not belong to the platform object
    WHVPS_INVALID_POINTER,               // An invalid pointer to a partition was passed to a function
    WHVPS_UNINITIALIZED,                 // The partition is not initialized
};

enum WHvVCPUStatus {
    WHVVCPUS_SUCCESS = 0,                // VCPU created successfully

    WHVVCPUS_FAILED = 0x80000000,        // The operation failed
    WHVVCPUS_CREATE_FAILED,              // Failed to create VCPU
    WHVVCPUS_CREATE_EMU_FAILED,          // Failed to create instruction emulator
    WHVVCPUS_DESTROY_EMU_FAILED,         // Failed to destroy the instruction emulator
    WHVVCPUS_NOT_INITIALIZED,            // Attempted to delete an uninitialized VCPU
    WHVVCPUS_ALREADY_INITIALIZED,        // Attempted to create an initialized VCPU
    WHVVCPUS_INVALID_POINTER,            // An invalid pointer to a VCPU was passed to a function
    WHVVCPUS_INVALID_OWNER,              // Attempted to delete a VCPU that does not belong to the partition
    WHVVCPUS_GET_REGS_FAILED,            // Failed to get registers
    WHVVCPUS_SET_REGS_FAILED,            // Failed to set registers
    WHVVCPUS_RUN_FAILED,                 // VCPU execution failed
    WHVVCPUS_EMULATION_FAILED,           // VCPU instruction emulation failed
};


#define WHV_IO_IN  0
#define WHV_IO_OUT 1


class WHvPartition;
class WHvVCPU;

class WinHvPlatform {
public:
    WinHvPlatform();
    ~WinHvPlatform();

    const bool IsPresent() const { return m_present; }

    WHvStatus GetCapability(WHV_CAPABILITY_CODE code, WHV_CAPABILITY *pCap);

    WHvPartitionStatus CreatePartition(WHvPartition **ppPartition);
    WHvPartitionStatus DeletePartition(WHvPartition **ppPartition);

private:
    bool m_present;

    std::vector<WHvPartition *> m_partitions;
};


class WHvPartition {
public:
    WHvPartitionStatus GetProperty(WHV_PARTITION_PROPERTY_CODE code, WHV_PARTITION_PROPERTY *ppProperty);
    WHvPartitionStatus SetProperty(WHV_PARTITION_PROPERTY_CODE code, WHV_PARTITION_PROPERTY *ppProperty);
    
    WHvPartitionStatus Setup();

    WHvPartitionStatus MapGpaRange(void *memory, WHV_GUEST_PHYSICAL_ADDRESS address, UINT64 size, WHV_MAP_GPA_RANGE_FLAGS flags);
    WHvPartitionStatus UnmapGpaRange(WHV_GUEST_PHYSICAL_ADDRESS address, UINT64 size);

    WHvVCPUStatus CreateVCPU(WHvVCPU **ppVcpu, UINT32 vpIndex);
    WHvVCPUStatus DeleteVCPU(WHvVCPU **ppVcpu);

    WHvPartitionStatus Close();

private:
    WHvPartition(WinHvPlatform *platform);
    ~WHvPartition();

    WHvPartitionStatus Initialize();

    WinHvPlatform *m_platform;
    WHV_PARTITION_HANDLE m_handle;

    std::vector<WHvVCPU *> m_vcpus;

    friend class WinHvPlatform;
};


typedef HRESULT (CALLBACK *WHvIoPortCallback)(PVOID Context, WHV_EMULATOR_IO_ACCESS_INFO* IoAccess);
typedef HRESULT (CALLBACK *WHvMemoryCallback)(PVOID Context, WHV_EMULATOR_MEMORY_ACCESS_INFO* MemoryAccess);

class WHvVCPU {
public:
    WHvVCPUStatus Run();
    WHvVCPUStatus CancelRun();

    WHvVCPUStatus Interrupt(uint16_t vector);

    WHvVCPUStatus GetRegisters(const WHV_REGISTER_NAME *regs, UINT32 count, WHV_REGISTER_VALUE *values);
    WHvVCPUStatus SetRegisters(const WHV_REGISTER_NAME *regs, UINT32 count, const WHV_REGISTER_VALUE *values);

    WHvVCPUStatus Close();

    const WHV_RUN_VP_EXIT_CONTEXT * ExitContext() const { return &m_exitContext; }

    void SetIoPortCallback(WHvIoPortCallback ioPortCallback) { m_ioPortCallback = ioPortCallback; }
    void SetMemoryCallback(WHvMemoryCallback memoryCallback) { m_memoryCallback = memoryCallback; }

private:
    WHvVCPU(WHV_PARTITION_HANDLE hPartition, UINT32 vpIndex);
    ~WHvVCPU();

    WHvVCPUStatus Initialize();

    bool m_initialized;

    WHV_PARTITION_HANDLE m_partitionHandle;
    UINT32 m_vpIndex;
    WHV_RUN_VP_EXIT_CONTEXT m_exitContext;

    WHV_EMULATOR_HANDLE m_emuHandle;
    WHvIoPortCallback m_ioPortCallback;
    WHvMemoryCallback m_memoryCallback;


    static HRESULT GetVirtualProcessorRegistersCallback(VOID* Context, const WHV_REGISTER_NAME* RegisterNames, UINT32 RegisterCount, WHV_REGISTER_VALUE* RegisterValues);
    static HRESULT SetVirtualProcessorRegistersCallback(VOID* Context, const WHV_REGISTER_NAME* RegisterNames, UINT32 RegisterCount, const WHV_REGISTER_VALUE* RegisterValues);
    static HRESULT TranslateGvaPageCallback(VOID* Context, WHV_GUEST_VIRTUAL_ADDRESS Gva, WHV_TRANSLATE_GVA_FLAGS TranslateFlags, WHV_TRANSLATE_GVA_RESULT_CODE* TranslationResult, WHV_GUEST_PHYSICAL_ADDRESS* Gpa);
    static HRESULT IoPortCallback(VOID* Context, WHV_EMULATOR_IO_ACCESS_INFO* IoAccess);
    static HRESULT MemoryCallback(VOID* Context, WHV_EMULATOR_MEMORY_ACCESS_INFO* MemoryAccess);


    friend class WHvPartition;
};
