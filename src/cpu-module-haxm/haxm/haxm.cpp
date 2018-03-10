#include "haxm.h"

Haxm::Haxm()
	: m_hHAXM(INVALID_HANDLE_VALUE)
	, m_lastError(ERROR_SUCCESS)
{
}

Haxm::~Haxm() {
	if (m_hHAXM != INVALID_HANDLE_VALUE) {
		// Close VMs
		for (auto it = m_vms.begin(); it != m_vms.end(); it++) {
			(*it)->Close();
			delete (*it);
		}
		m_vms.clear();

		CloseHandle(m_hHAXM);
		m_hHAXM = INVALID_HANDLE_VALUE;
	}
}

HaxmStatus Haxm::Initialize() {
	if (m_hHAXM != INVALID_HANDLE_VALUE) {
		return HXS_SUCCESS;
	}

	// Open the device
	m_hHAXM = CreateFileW(L"\\\\.\\HAX", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hHAXM == INVALID_HANDLE_VALUE) {
		m_lastError = GetLastError();
		return HXS_NOT_FOUND;
	}

	DWORD returnSize;
	BOOLEAN bResult;

	// Retrieve version
	bResult = DeviceIoControl(m_hHAXM,
		HAX_IOCTL_VERSION,
		NULL, 0,
		&m_ver, sizeof(m_ver),
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		CloseHandle(m_hHAXM);
		m_hHAXM = INVALID_HANDLE_VALUE;
		m_lastError = GetLastError();
		return HXS_INIT_FAILED;
	}

	// Retrieve capabilities
	bResult = DeviceIoControl(m_hHAXM,
		HAX_IOCTL_CAPABILITY,
		NULL, 0,
		&m_caps, sizeof(m_caps),
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		CloseHandle(m_hHAXM);
		m_hHAXM = INVALID_HANDLE_VALUE;
		m_lastError = GetLastError();
		return HXS_INIT_FAILED;
	}

	return HXS_SUCCESS;
}

HaxmStatus Haxm::SetGlobalMemoryLimit(bool enabled, uint64_t limitMB) {
	DWORD returnSize;
	BOOLEAN bResult;

	struct hax_set_memlimit memlimit;
	memlimit.enable_memlimit = enabled;
	memlimit.memory_limit = enabled ? limitMB : 0;
	bResult = DeviceIoControl(m_hHAXM,
		HAX_IOCTL_SET_MEMLIMIT,
		&memlimit, sizeof(memlimit),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXS_SET_MEM_LIMIT_FAILED;
	}
	return HXS_SUCCESS;
}

HaxmVMStatus Haxm::CreateVM(HaxmVM **vm) {
	*vm = new HaxmVM(*this);
	HaxmVMStatus status = (*vm)->Initialize();
	if (status != HXS_SUCCESS) {
		m_lastError = GetLastError();
		delete *vm;
		*vm = nullptr;
	}
	else {
		m_vms.push_back(*vm);
	}
	return status;
}

// ----------------------------------------------------------------------------

HaxmVM::HaxmVM(Haxm& haxm)
	: m_haxm(haxm)
	, m_hVM(INVALID_HANDLE_VALUE)
	, m_vmID(-1)
	, m_fastMMIO(false)
	, m_nextVCPUID(0)
	, m_lastError(ERROR_SUCCESS)
{
}

HaxmVM::~HaxmVM() {
	Close();
}

HaxmVMStatus HaxmVM::Initialize() {
	DWORD returnSize;
	BOOLEAN bResult;
	
	// Ask HAXM to create a VM
	bResult = DeviceIoControl(m_haxm.Handle(),
		HAX_IOCTL_CREATE_VM,
		NULL, 0,
		&m_vmID, sizeof(m_vmID),
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		return HXVMS_CREATE_FAILED;
	}

	// VM created successfully; open the object
	wchar_t vmName[13];
	swprintf_s(vmName, L"\\\\.\\hax_vm%02d", m_vmID);
	m_hVM = CreateFileW(vmName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hVM == INVALID_HANDLE_VALUE) {
		m_vmID = -1;
		Close();
		return HXVMS_CREATE_FAILED;
	}

	// Report QEMU API version 4 to enable fast MMIO
	if (m_haxm.GetCapabilities()->winfo & HAX_CAP_FASTMMIO) {
		struct hax_qemu_version qversion;
		qversion.cur_version = 0x4;
		qversion.least_version = 0x1;
		bResult = DeviceIoControl(m_hVM,
			HAX_VM_IOCTL_NOTIFY_QEMU_VERSION,
			&qversion, sizeof(qversion),
			NULL, 0,
			&returnSize,
			(LPOVERLAPPED)NULL);

		m_fastMMIO = bResult;
	}

	return HXVMS_SUCCESS;
}

HaxmVMStatus HaxmVM::AllocateMemory(void *memoryBlock, uint32_t memorySize, uint64_t basePhysicalAddress, HaxmVMMemoryType memoryType) {
	// Memory block must be aligned to 4 KB pages
	if (((uint64_t)memoryBlock) & 0xFFF) {
		return HXVMS_MEM_MISALIGNED;
	}

	// Memory size must be aligned to 4 KB pages
	if (memorySize & 0xFFF) {
		return HXVMS_MEMSIZE_MISALIGNED;
	}

	DWORD returnSize;
	BOOLEAN bResult;

	// Allocate memory
	struct hax_alloc_ram_info memInfo;
	memInfo.va = (uint64_t)memoryBlock;
	memInfo.size = memorySize;
	bResult = DeviceIoControl(m_hVM,
		HAX_VM_IOCTL_ALLOC_RAM,
		&memInfo, sizeof(memInfo),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVMS_ALLOC_MEM_FAILED;
	}

	// Configure memory
	struct hax_set_ram_info setMemInfo;
	setMemInfo.pa_start = basePhysicalAddress;
	setMemInfo.va = (uint64_t)memoryBlock;
	setMemInfo.size = memorySize;
	switch (memoryType) {
	case HXVM_MEM_RAM: setMemInfo.flags = 0; break;
	case HXVM_MEM_ROM: setMemInfo.flags = HAX_RAM_INFO_ROM; break;
	}
	
	bResult = DeviceIoControl(m_hVM,
		HAX_VM_IOCTL_SET_RAM,
		&setMemInfo, sizeof(setMemInfo),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVMS_SET_MEM_FAILED;
	}

	return HXVMS_SUCCESS;
}

HaxmVMStatus HaxmVM::AllocateMemory64(void *memoryBlock, uint64_t memorySize, uint64_t basePhysicalAddress, HaxmVMMemoryType memoryType) {
	// HAXM module must support 64-bit memory operations
	if ((m_haxm.GetCapabilities()->winfo & HAX_CAP_64BIT_SETRAM) == 0) {
		return HXVMS_UNSUPPORTED;
	}

	// Memory block must be aligned to 4 KB pages
	if (((uint64_t)memoryBlock) & 0xFFF) {
		return HXVMS_MEM_MISALIGNED;
	}

	// Memory size must be aligned to 4 KB pages
	if (memorySize & 0xFFF) {
		return HXVMS_MEMSIZE_MISALIGNED;
	}

	DWORD returnSize;
	BOOLEAN bResult;

	// Allocate memory
	struct hax_ramblock_info memInfo;
	memInfo.start_va = (uint64_t)memoryBlock;
	memInfo.size = memorySize;
	memInfo.reserved = 0;
	bResult = DeviceIoControl(m_hVM,
		HAX_VM_IOCTL_ADD_RAMBLOCK,
		&memInfo, sizeof(memInfo),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVMS_ALLOC_MEM_FAILED;
	}

	// Configure memory
	struct hax_set_ram_info2 setMemInfo;
	setMemInfo.pa_start = basePhysicalAddress;
	setMemInfo.va = (uint64_t)memoryBlock;
	setMemInfo.size = memorySize;
	setMemInfo.reserved1 = setMemInfo.reserved2 = 0;
	switch (memoryType) {
	case HXVM_MEM_RAM: setMemInfo.flags = 0; break;
	case HXVM_MEM_ROM: setMemInfo.flags = HAX_RAM_INFO_ROM; break;
	}

	bResult = DeviceIoControl(m_hVM,
		HAX_VM_IOCTL_SET_RAM2,
		&setMemInfo, sizeof(setMemInfo),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVMS_SET_MEM_FAILED;
	}

	return HXVMS_SUCCESS;
}

HaxmVMStatus HaxmVM::FreeMemory(uint64_t basePhysicalAddress, uint32_t size) {
	// HAXM API version must be 4 or greater to support this operation
	if (m_haxm.GetModuleVersion()->cur_version < 4) {
		return HXVMS_UNSUPPORTED;
	}

	DWORD returnSize;
	BOOLEAN bResult;

	struct hax_set_ram_info setMemInfo;
	setMemInfo.pa_start = basePhysicalAddress;
	setMemInfo.va = 0;
	setMemInfo.size = size;
	setMemInfo.flags = HAX_RAM_INFO_INVALID;

	bResult = DeviceIoControl(m_hVM,
		HAX_VM_IOCTL_SET_RAM,
		&setMemInfo, sizeof(setMemInfo),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVMS_FREE_MEM_FAILED;
	}

	return HXVMS_SUCCESS;
}

HaxmVMStatus HaxmVM::FreeMemory64(uint64_t basePhysicalAddress, uint64_t size) {
	// HAXM module must support 64-bit memory operations
	if ((m_haxm.GetCapabilities()->winfo & HAX_CAP_64BIT_SETRAM) == 0) {
		return HXVMS_UNSUPPORTED;
	}

	DWORD returnSize;
	BOOLEAN bResult;

	struct hax_set_ram_info2 setMemInfo;
	setMemInfo.pa_start = basePhysicalAddress;
	setMemInfo.va = 0;
	setMemInfo.size = size;
	setMemInfo.reserved1 = setMemInfo.reserved2 = 0;
	setMemInfo.flags = HAX_RAM_INFO_INVALID;

	bResult = DeviceIoControl(m_hVM,
		HAX_VM_IOCTL_SET_RAM2,
		&setMemInfo, sizeof(setMemInfo),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVMS_FREE_MEM_FAILED;
	}

	return HXVMS_SUCCESS;
}

HaxmVCPUStatus HaxmVM::CreateVCPU(HaxmVCPU **vcpu) {
	*vcpu = new HaxmVCPU(*this, m_nextVCPUID++);
	HaxmVCPUStatus status = (*vcpu)->Initialize();
	if (status != HXS_SUCCESS) {
		m_lastError = GetLastError();
		delete *vcpu;
		*vcpu = nullptr;
	}
	else {
		m_vcpus.push_back(*vcpu);
	}
	return status;
}

HaxmVCPUStatus HaxmVM::FreeVCPU(HaxmVCPU **vcpu) {
	for (auto it = m_vcpus.begin(); it != m_vcpus.end(); it++) {
		if (*it == *vcpu) {
			(*vcpu)->Close();
			m_vcpus.erase(it);
			delete *vcpu;
			*vcpu = nullptr;
			return HXVCPUS_SUCCESS;
		}
	}

	return HXVCPUS_FAILED;
}

HaxmVMStatus HaxmVM::Close() {
	if (m_hVM != INVALID_HANDLE_VALUE) {
		// Close VCPUs
		for (auto it = m_vcpus.begin(); it != m_vcpus.end(); it++) {
			(*it)->Close();
			delete (*it);
		}
		m_vcpus.clear();

		CloseHandle(m_hVM);
		m_hVM = INVALID_HANDLE_VALUE;
	}

	return HXVMS_SUCCESS;
}

// ----------------------------------------------------------------------------

HaxmVCPU::HaxmVCPU(HaxmVM& vm, uint32_t id)
	: m_vm(vm)
	, m_vcpuID(id)
	, m_hVCPU(INVALID_HANDLE_VALUE)
	, m_lastError(ERROR_SUCCESS)
{
}

HaxmVCPU::~HaxmVCPU() {
}

HaxmVCPUStatus HaxmVCPU::Initialize() {
	DWORD returnSize;
	BOOLEAN bResult;

	// Create VCPU
	bResult = DeviceIoControl(m_vm.Handle(),
		HAX_VM_IOCTL_VCPU_CREATE,
		&m_vcpuID, sizeof(m_vcpuID),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVCPUS_CREATE_FAILED;
	}

	// Open VCPU object
	wchar_t vcpuName[20];
	swprintf_s(vcpuName, L"\\\\.\\hax_vm%02d_vcpu%02d", m_vm.ID(), m_vcpuID);
	m_hVCPU = CreateFileW(vcpuName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hVCPU == INVALID_HANDLE_VALUE) {
		m_lastError = GetLastError();
		Close();
		return HXVCPUS_CREATE_FAILED;
	}

	// Setup tunnel
	struct hax_tunnel_info tunnelInfo;
	bResult = DeviceIoControl(m_hVCPU,
		HAX_VCPU_IOCTL_SETUP_TUNNEL,
		NULL, 0,
		&tunnelInfo, sizeof(tunnelInfo),
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		Close();
		return HXVCPUS_TUNNEL_SETUP_FAILED;
	}

	m_tunnel = (struct hax_tunnel *)(intptr_t)tunnelInfo.va;
	m_ioTunnel = (unsigned char *)(intptr_t)tunnelInfo.io_va;

	return HXVCPUS_SUCCESS;
}

HaxmVCPUStatus HaxmVCPU::GetRegisters(struct vcpu_state_t *registers) {
	DWORD returnSize;
	BOOLEAN bResult;

	bResult = DeviceIoControl(m_hVCPU,
		HAX_VCPU_GET_REGS,
		NULL, 0,
		registers, sizeof(struct vcpu_state_t),
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVCPUS_FAILED;
	}

	return HXVCPUS_SUCCESS;
}

HaxmVCPUStatus HaxmVCPU::SetRegisters(struct vcpu_state_t *registers) {
	DWORD returnSize;
	BOOLEAN bResult;

	bResult = DeviceIoControl(m_hVCPU,
		HAX_VCPU_SET_REGS,
		registers, sizeof(struct vcpu_state_t),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVCPUS_FAILED;
	}

	return HXVCPUS_SUCCESS;
}

HaxmVCPUStatus HaxmVCPU::GetFPURegisters(struct fx_layout *registers) {
	DWORD returnSize;
	BOOLEAN bResult;

	bResult = DeviceIoControl(m_hVCPU,
		HAX_VCPU_IOCTL_GET_FPU,
		NULL, 0,
		registers, sizeof(struct fx_layout),
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVCPUS_FAILED;
	}

	return HXVCPUS_SUCCESS;
}

HaxmVCPUStatus HaxmVCPU::SetFPURegisters(struct fx_layout *registers) {
	DWORD returnSize;
	BOOLEAN bResult;

	bResult = DeviceIoControl(m_hVCPU,
		HAX_VCPU_IOCTL_SET_FPU,
		registers, sizeof(struct fx_layout),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVCPUS_FAILED;
	}

	return HXVCPUS_SUCCESS;
}

HaxmVCPUStatus HaxmVCPU::GetMSRs(struct hax_msr_data *msrData) {
	DWORD returnSize;
	BOOLEAN bResult;

	bResult = DeviceIoControl(m_hVCPU,
		HAX_VCPU_IOCTL_GET_MSRS,
		msrData, sizeof(struct hax_msr_data),
		msrData, sizeof(struct hax_msr_data),
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVCPUS_FAILED;
	}

	return HXVCPUS_SUCCESS;
}

HaxmVCPUStatus HaxmVCPU::SetMSRs(struct hax_msr_data *msrData) {
	DWORD returnSize;
	BOOLEAN bResult;

	bResult = DeviceIoControl(m_hVCPU,
		HAX_VCPU_IOCTL_SET_MSRS,
		msrData, sizeof(struct hax_msr_data),
		msrData, sizeof(struct hax_msr_data),
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVCPUS_FAILED;
	}

	return HXVCPUS_SUCCESS;
}

HaxmVCPUStatus HaxmVCPU::Run() {
	DWORD returnSize;
	BOOLEAN bResult;

	bResult = DeviceIoControl(m_hVCPU,
		HAX_VCPU_IOCTL_RUN,
		NULL, 0,
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVCPUS_FAILED;
	}

	return HXVCPUS_SUCCESS;
}

HaxmVCPUStatus HaxmVCPU::Interrupt(uint8_t vector) {
	DWORD returnSize;
	BOOLEAN bResult;

	uint32_t vec32 = vector;
	bResult = DeviceIoControl(m_hVCPU,
		HAX_VCPU_IOCTL_INTERRUPT,
		&vec32, sizeof(vec32),
		NULL, 0,
		&returnSize,
		(LPOVERLAPPED)NULL);
	if (!bResult) {
		m_lastError = GetLastError();
		return HXVCPUS_FAILED;
	}

	return HXVCPUS_SUCCESS;
}

HaxmVCPUStatus HaxmVCPU::Close() {
	if (m_hVCPU != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hVCPU);
		m_hVCPU = INVALID_HANDLE_VALUE;
	}

	return HXVCPUS_SUCCESS;
}
