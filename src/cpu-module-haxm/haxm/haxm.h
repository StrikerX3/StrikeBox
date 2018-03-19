#pragma once

#include "hax_interface.h"

#include <Windows.h>
#include <vector>

enum HaxmStatus {
	HXS_SUCCESS = 0,                 // HAXM module initialized and loaded successfully
	HXS_NOT_FOUND = 0x80000000,      // HAXM module not found
	HXS_INIT_FAILED,                 // HAXM initialization failed

	HXS_SET_MEM_LIMIT_FAILED,        // Could not set the global memory limit
};

enum HaxmVMStatus {
	HXVMS_SUCCESS = 0,               // VM created successfully or the operation completed without errors
	HXVMS_UNSUPPORTED = 0x90000000,  // The operation is unsupported

	HXVMS_CREATE_FAILED,             // Failed to create VM

	HXVMS_MEM_MISALIGNED,            // Memory block is not aligned to 4 KB pages
	HXVMS_MEMSIZE_MISALIGNED,        // Memory size is not aligned to 4 KB pages
	HXVMS_ALLOC_MEM_FAILED,          // Failed to allocate memory
	HXVMS_SET_MEM_FAILED,            // Failed to configure memory
	HXVMS_FREE_MEM_FAILED,           // Failed to free memory
};

enum HaxmVCPUStatus {
	HXVCPUS_SUCCESS = 0,          // VCPU created successfully or the operation completed without errors

	HXVCPUS_FAILED = 0xa0000000,  // The operation failed

	HXVCPUS_CREATE_FAILED,        // Failed to create VCPU
	HXVCPUS_TUNNEL_SETUP_FAILED,  // Failed to setup VCPU tunnel
};

enum HaxmVMMemoryType {
	HXVM_MEM_RAM = 0,
	HXVM_MEM_ROM,
};

class HaxmVM;
class HaxmVCPU;

class Haxm {
public:
	Haxm();
	~Haxm();

	HaxmStatus Initialize();

	HaxmStatus SetGlobalMemoryLimit(bool enabled, uint64_t limitMB);

	HaxmVMStatus CreateVM(HaxmVM **vm);

	const struct hax_module_version* GetModuleVersion() const { return &m_ver; }
	const struct hax_capabilityinfo* GetCapabilities() const { return &m_caps; }

	const HANDLE Handle() const { return m_hHAXM; }
	const DWORD GetLastError() const { return m_lastError; }
private:
	HANDLE m_hHAXM;
	DWORD m_lastError;
	struct hax_module_version m_ver;
	struct hax_capabilityinfo m_caps;

	std::vector<HaxmVM *> m_vms;
};

class HaxmVM {
public:
	HaxmVMStatus AllocateMemory(void *memoryBlock, uint32_t memorySize, uint64_t basePhysicalAddress, HaxmVMMemoryType memoryType);
	HaxmVMStatus AllocateMemory64(void *memoryBlock, uint64_t memorySize, uint64_t basePhysicalAddress, HaxmVMMemoryType memoryType);

	HaxmVMStatus FreeMemory(uint64_t basePhysicalAddress, uint32_t size);
	HaxmVMStatus FreeMemory64(uint64_t basePhysicalAddress, uint64_t size);

	HaxmVCPUStatus CreateVCPU(HaxmVCPU **vcpu);
	HaxmVCPUStatus FreeVCPU(HaxmVCPU **vcpu);

	HaxmVMStatus Close();

	const bool FastMMIOEnabled() const { return m_fastMMIO; }

	const uint32_t ID() const { return m_vmID; }
	const HANDLE Handle() const { return m_hVM; }
	const DWORD GetLastError() const { return m_lastError; }
private:
	// Used internally by Haxm to initialize this object
	HaxmVM(Haxm& haxm);
	~HaxmVM();

	HaxmVMStatus Initialize();

	bool m_fastMMIO;
	uint32_t m_nextVCPUID;

	uint32_t m_vmID;
	HANDLE m_hVM;
	DWORD m_lastError;

	std::vector<HaxmVCPU *> m_vcpus;

	Haxm& m_haxm;

	friend class Haxm;
};

class HaxmVCPU {
public:
	HaxmVCPUStatus GetRegisters(struct vcpu_state_t *registers);
	HaxmVCPUStatus SetRegisters(struct vcpu_state_t *registers);

	HaxmVCPUStatus GetFPURegisters(struct fx_layout *registers);
	HaxmVCPUStatus SetFPURegisters(struct fx_layout *registers);
	
	HaxmVCPUStatus GetMSRs(struct hax_msr_data *msrData);
	HaxmVCPUStatus SetMSRs(struct hax_msr_data *msrData);

	HaxmVCPUStatus Run();

	HaxmVCPUStatus Interrupt(uint8_t vector);

	HaxmVCPUStatus Close();

	struct hax_tunnel* Tunnel() const { return m_tunnel; }
	unsigned char* IOTunnel() const { return m_ioTunnel; }

	const uint32_t ID() const { return m_vcpuID; }
	const HANDLE Handle() const { return m_hVCPU; }
	const DWORD GetLastError() const { return m_lastError; }
private:
	// Used internally by HaxmVM to initialize this object
	HaxmVCPU(HaxmVM& vm, uint32_t id);
	~HaxmVCPU();

	HaxmVCPUStatus Initialize();

	struct hax_tunnel *m_tunnel;
	unsigned char *m_ioTunnel;

	uint32_t m_vcpuID;
	HANDLE m_hVCPU;
	DWORD m_lastError;
//
	HaxmVM& m_vm;

	friend class HaxmVM;
};
