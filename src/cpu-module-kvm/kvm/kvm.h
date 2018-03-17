#pragma once

#include <vector>
#include <linux/kvm.h>
#include <cstdint>

enum KvmStatus {
    KVMS_SUCCESS,
    KVMS_OPEN_FAILED,
    KVMS_API_VERSION_UNSUPPORTED,
    KVMS_MISSING_CAP
};

enum KvmVMStatus {
    KVMVMS_SUCCESS,
    KVMVMS_CREATE_FAILED,
    KVMVMS_MEM_MISALIGNED,
    KVMVMS_MEMSIZE_MISALIGNED,
    KVMVMS_MEM_ERROR,
    KVMVMS_MEM_NOT_FOUND
};

enum KvmVCPUStatus {
    KVMVCPUS_SUCCESS,
    KVMVCPUS_CREATE_FAILED
};

typedef struct {
    uint32_t size;
    uint64_t startAddr;
    struct kvm_userspace_memory_region memoryRegion;
} KvmMemoryRecord;

class KvmVM;
class KvmVCPU;

class Kvm {
public:
    Kvm();
    ~Kvm();

    KvmStatus Initialize();
    KvmVMStatus CreateVM(KvmVM **vm);
    const int handle() const { return m_fd; }

private:
    int m_fd;
    std::vector<KvmVM*> m_vms;
};

class KvmVM {
public:
    KvmVMStatus MapUserMemoryToGuest(void *userMemoryBlock, uint32_t userMemorySize, uint32_t guestBaseAddress);
    const int handle() const { return m_fd; }
    const int kvmHandle() const { return m_kvm.handle(); }

private:
    KvmVM(Kvm &kvm);
    ~KvmVM();

    KvmVMStatus Initialize();

    int m_fd;
    std::vector<KvmVCPU*> m_vcpus;
    std::vector<KvmMemoryRecord*> m_memoryRecords;

    Kvm& m_kvm;

    friend class Kvm;
};

class KvmVCPU {
private:
    KvmVCPU(KvmVM &vm, uint32_t id);
    ~KvmVCPU();

    KvmVCPUStatus Initialize();

    KvmVM& m_vm;

    uint32_t m_vcpuID;
    int m_fd;

    struct kvm_regs m_regs;
    struct kvm_sregs m_sregs;
    struct kvm_run* m_kvmRun;
    int m_kvmRunMmapSize;

    friend class KvmVM;
};