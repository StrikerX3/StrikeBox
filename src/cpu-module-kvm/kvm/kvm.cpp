#include "kvm.h"

#include <fcntl.h>
#include <sys/ioctl.h>

Kvm::Kvm() {

}

Kvm::~Kvm() {

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

}

KvmVMStatus KvmVM::Initialize() {
    // Create the VM
    m_fd = ioctl(m_kvm.m_fd, KVM_CREATE_VM, 0);
    if(m_fd < 0) {
        return KVMVMS_CREATE_FAILED;
    }

    // Move the identity map. It usually resides at
    // 0xfffbc000 which will conflict with the xbox bios.
    uint32_t = identityMapAddr = 0xD0000000;
    if(ioctl(m_fd, KVM_SET_IDENTITY_MAP_ADDR, &identityMapAddr) < 0) {
        return KVMVMS_CREATE_FAILED;
    }
}