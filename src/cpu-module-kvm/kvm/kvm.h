#pragma once

#include <vector>
#include <linux/kvm.h>

enum KvmStatus {
    KVMS_SUCCESS,
    KVMS_API_VERSION_UNSUPPORTED,
    KVMS_MISSING_CAP
};

enum KvmVMStatus {
    KVMVMS_SUCCESS,
    KVMVMS_CREATE_FAILED
};

enum KvmVCPUStatus {
    KVMVCPUS_SUCCESS,
    KVMVCPUS_CREATE_FAILED
};

class KvmVM;
class KvmVCPU;

class Kvm {
public:
    Kvm();
    ~Kvm();

    KvmStatus Initialize();
    KvmVMStatus CreateVM(KvmVM **vm);
private:
    int m_fd;

private:
    std::vector<KvmVM*> m_vms;
};

class KvmVM {
public:
    const int handle() const { return m_fd; }
private:
    KvmVM(Kvm &kvm);
    ~KvmVM();

    KvmVMStatus Initialize();

    int m_fd;
    std::vector<KvmVCPU*> m_vcpus;

    Kvm& m_kvm;

    friend class Kvm;
};

class KvmVCPU {

};