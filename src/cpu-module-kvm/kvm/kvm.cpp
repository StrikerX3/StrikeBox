#include "kvm.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

Kvm::Kvm() {

}

Kvm::~Kvm() {
    if(m_fd > 0) {
        close(m_fd);
    }
}

KvmStatus Kvm::Initialize() {
    // Open the KVM module
    m_fd = open("/dev/kvm", O_RDWR);

    if(m_fd < 0) {
        return KVMS_OPEN_FAILED;
    }

    // Get KVM version
    int kvmVersion;

    // Refuse to run if API version != 12 as per API docs recommendation.
    kvmVersion = ioctl(m_fd, KVM_GET_API_VERSION, nullptr);
    if(kvmVersion != 12) {
        return KVMS_API_VERSION_UNSUPPORTED;
    }

    // Check if have the capabilities we need.
    int kvmCapResult;

    // User mem to VM mapping.
    kvmCapResult = ioctl(m_fd, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
    if(kvmCapResult == 0){
        return KVMS_MISSING_CAP;
    }

    // Set identity map address.
    kvmCapResult = ioctl(m_fd, KVM_CHECK_EXTENSION, KVM_CAP_SET_IDENTITY_MAP_ADDR);
    if(kvmCapResult == 0) {
        return KVMS_MISSING_CAP;
    }

    // If we've got this far, we're all good.
    return KVMS_SUCCESS;
}

KvmVMStatus Kvm::CreateVM(KvmVM **vm) {
    *vm = new KvmVM(*this);
    KvmVMStatus status = (*vm)->Initialize();
    if(status != KVMVMS_SUCCESS) {
        delete *vm;
        *vm = nullptr;
    } else {
        m_vms.push_back(*vm);
    }
    return status;
}

// --------------------------------------------
KvmVM::KvmVM(Kvm &kvm) :
    m_kvm(kvm)
{
}

KvmVM::~KvmVM() {
    if(m_fd > 0) {
        close(m_fd);
    }
}

KvmVMStatus KvmVM::Initialize() {
    // Create the VM
    m_fd = ioctl(m_kvm.handle(), KVM_CREATE_VM, 0);
    if(m_fd < 0) {
        return KVMVMS_CREATE_FAILED;
    }

    // Move the identity map. It usually resides at
    // 0xfffbc000 which will conflict with the xbox bios.
    uint32_t identityMapAddr = 0xD0000000;
    if(ioctl(m_fd, KVM_SET_IDENTITY_MAP_ADDR, &identityMapAddr) < 0) {
        return KVMVMS_CREATE_FAILED;
    }

    return KVMVMS_SUCCESS;
}

KvmVCPUStatus KvmVM::CreateVCPU(KvmVCPU **vcpu) {
    *vcpu = new KvmVCPU(*this, m_vcpus.size());
    KvmVCPUStatus status = (*vcpu)->Initialize();

    if(status != KVMVCPUS_SUCCESS) {
        delete *vcpu;
        *vcpu = nullptr;
    } else {
        m_vcpus.push_back(*vcpu);
    }
    return status;
}

KvmVMStatus KvmVM::MapUserMemoryToGuest(void *userMemoryBlock, uint32_t userMemorySize, uint32_t guestBaseAddress) {

    if(((uint64_t)userMemoryBlock) & 0xFFF) {
        return KVMVMS_MEM_MISALIGNED;
    }

    if(userMemorySize & 0xFFF) {
        return KVMVMS_MEMSIZE_MISALIGNED;
    }

    KvmMemoryRecord *memoryRecord = new KvmMemoryRecord;
    memoryRecord->size = userMemorySize;
    memoryRecord->startAddr = (uint64_t)userMemoryBlock;
    memoryRecord->memoryRegion.memory_size = userMemorySize;
    memoryRecord->memoryRegion.userspace_addr = (uint64_t)userMemoryBlock;
    memoryRecord->memoryRegion.guest_phys_addr = (uint64_t)guestBaseAddress;
    memoryRecord->memoryRegion.slot = (uint32_t)m_memoryRecords.size();

    if(ioctl(m_fd, KVM_SET_USER_MEMORY_REGION, &memoryRecord->memoryRegion) < 0) {
        return KVMVMS_MEM_ERROR;
    }

    m_memoryRecords.push_back(memoryRecord);

    return KVMVMS_SUCCESS;
}

// --------------------------------------------------------------------
KvmVCPU::KvmVCPU(KvmVM& vm, uint32_t id) :
    m_vm(vm), m_vcpuID(id)
{

}

KvmVCPU::~KvmVCPU() {
    if(m_fd > 0) {
        close(m_fd);
    }

    if(m_kvmRun) {
        munmap(m_kvmRun, m_kvmRunMmapSize);
    }
}

KvmVCPUStatus KvmVCPU::Initialize() {

    // Create the VCPU.
    m_fd = ioctl(m_vm.handle(), KVM_CREATE_VCPU, m_vcpuID);
    if(m_fd < 0) {
        return KVMVCPUS_CREATE_FAILED;
    }

    // Get the kvmRun struct size.
    m_kvmRunMmapSize = ioctl(m_vm.kvmHandle(), KVM_GET_VCPU_MMAP_SIZE, 0);
    if(m_kvmRunMmapSize < 0) {
        return KVMVCPUS_CREATE_FAILED;
    }

    m_kvmRun = (struct kvm_run*)mmap(nullptr, (size_t)m_kvmRunMmapSize,
                                     PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);

    if(m_kvmRun < 0) {
        return KVMVCPUS_CREATE_FAILED;
    }

    return KVMVCPUS_SUCCESS;
}

KvmVCPUStatus KvmVCPU::Run() {
    // Start running!
    if(ioctl(m_fd, KVM_RUN, 0) < 0) {
        return KVMVCPUS_RUN_FAILED;
    }

    // Update our CPU regs.
//    if(ioctl(m_fd, KVM_GET_REGS, &m_regs) < 0) {
//        return KVMVCPUS_REG_READ_ERROR;
//    }
//
//    if(ioctl(m_fd, KVM_GET_SREGS, &m_sregs) < 0) {
//        return KVMVCPUS_REG_READ_ERROR;
//    }

    return KVMVCPUS_SUCCESS;
}

