#include <assert.h>
#include "cpu_uc.h"
#include "log.h"

// Map Xbox CPU Register ids to Unicorn ids
const int kCpuRegisterXboxToUnicorn[REG_MAX] = {
    UC_X86_REG_EIP,    // REG_EIP
    UC_X86_REG_EFLAGS, // REG_EFLAGS
    UC_X86_REG_EAX,    // REG_EAX
    UC_X86_REG_ECX,    // REG_ECX
    UC_X86_REG_EDX,    // REG_EDX
    UC_X86_REG_EBX,    // REG_EBX
    UC_X86_REG_ESI,    // REG_ESI
    UC_X86_REG_EDI,    // REG_EDI
    UC_X86_REG_ESP,    // REG_ESP
    UC_X86_REG_EBP,    // REG_EBP
    UC_X86_REG_CS,     // REG_CS
    UC_X86_REG_SS,     // REG_SS
    UC_X86_REG_DS,     // REG_DS
    UC_X86_REG_ES,     // REG_ES
    UC_X86_REG_FS,     // REG_FS
    UC_X86_REG_GS,     // REG_GS
    -1,                // FIXME: REG_IDT_BASE
    -1,                // FIXME: REG_IDT_LIMIT
    -1,                // FIXME: REG_GDT_BASE
    -1,                // FIXME: REG_GDT_LIMIT
    UC_X86_REG_CR0,    // REG_CR0
    UC_X86_REG_CR1,    // REG_CR1
    UC_X86_REG_CR2,    // REG_CR2
    UC_X86_REG_CR3,    // REG_CR3
    UC_X86_REG_CR4,    // REG_CR4
};

/*!
 * Constructor
 */
UnicornCpu::UnicornCpu()
{
    m_uc = NULL;
}

/*!
 * Destructor
 */
UnicornCpu::~UnicornCpu()
{
    if (m_uc) {
        uc_close(m_uc);
    }
}

/*!
 * Unicorn Error Handler
 */
static bool xb_uc_err_hook(
    uc_engine   *uc,
    uc_mem_type  type,
    uint64_t     address,
    int          size,
    int64_t      value,
    void        *user_data
    )
{
    const char *at;
    if (type == UC_MEM_READ_UNMAPPED) {
        at = "UNMAPPED READ";
    } else if (type == UC_MEM_WRITE_UNMAPPED) {
        at = "UNMAPPED WRITE";
    }
    log_error("BAD MEMORY ACCESS (%s) @ %08llx", at, address);
    if (type == UC_MEM_WRITE_UNMAPPED) {
        log_error(" = %08llx (%d)", value, size);
    }
    log_error("\n");
    return false;
}

/*!
 * Unicorn Interrupt Handler
 */
void UnicornCpu::xb_uc_intr_hook(
    uc_engine *uc,
    uint32_t   intno,
    void      *user_data
    )
{
    UnicornCpu *inst = (UnicornCpu *)user_data;
    inst->m_exit_info.reason = EXIT_INTERRUPT;
    inst->m_exit_info.intr_vector = intno;
    uc_emu_stop(uc);
}

/*!
 * Initialize the CPU
 */
int UnicornCpu::Initialize()
{
    uc_err err;
    uc_hook hook_err;
    uc_hook hook_intr;
    
    // Initialize emulator in X86-32bit mode
    err = uc_open(UC_ARCH_X86, UC_MODE_32, &m_uc);

    if (err != UC_ERR_OK) {
        log_error("uc_open failed (%u)\n", err);
        return -1;
    }

    err = uc_hook_add(m_uc, &hook_err, // FIXME: We leak these hooks! They should be cleaned up.
                      UC_HOOK_MEM_UNMAPPED, (void*)xb_uc_err_hook,
                      NULL, 0, 0xffffffff);
    if (err != UC_ERR_OK) {
        log_error("uc_hook_add failed (%u)\n", err);
        return -1;
    }

    err = uc_hook_add(m_uc, &hook_intr, // FIXME: We leak these hooks! They should be cleaned up.
                      UC_HOOK_INTR, (void*)&UnicornCpu::xb_uc_intr_hook,
                      this, 0, 0xffffffff);
    if (err != UC_ERR_OK) {
        log_error("uc_hook_add failed (%u)\n", err);
        return -1;
    }

    return 0;
}

/*!
 * Read from memory
 */
int UnicornCpu::MemRead(uint32_t addr, size_t size, void *value)
{
    uc_mem_read(m_uc, addr, value, size);
    return 0;
}

/*!
 * Write to memory
 */
int UnicornCpu::MemWrite(uint32_t addr, size_t size, void *value)
{
    uc_mem_write(m_uc, addr, value, size);
    return 0;
}

/*!
 * Read from a register
 */
int UnicornCpu::RegRead(enum CpuReg reg, uint32_t *value)
{
    uc_reg_read(m_uc, kCpuRegisterXboxToUnicorn[reg], value);
    return 0;
}

/*!
 * Write to a register
 */
int UnicornCpu::RegWrite(enum CpuReg reg, uint32_t value)
{
    uc_reg_write(m_uc, kCpuRegisterXboxToUnicorn[reg], &value);
    return 0;
}

/*!
 * Run the CPU for a given amount of time
 */
int UnicornCpu::Run(uint64_t time_limit_us)
{
    uc_err err;
    uint32_t eip;

    // Get current EIP
    RegRead(REG_EIP, &eip);

    // Begin CPU emulation
    m_exit_info.reason = EXIT_NORMAL;
    err = uc_emu_start(m_uc, eip, 0, time_limit_us, 0);
    // If an interrupt occured, exit reason will have been updated
    if (err != UC_ERR_OK) {
        m_exit_info.reason = EXIT_ERROR;
        log_error("Failed on uc_emu_start() with error returned %u: %s\n",
        err, uc_strerror(err));
        return -1;
    }

    return 0;
}

/*!
 * Get info about why the VM exited
 */
struct CpuExitInfo *UnicornCpu::GetExitInfo()
{
    return &m_exit_info;
}

/*!
 * Unicorn MMIO handler
 */
static void xb_uc_hook(
    uc_engine   *uc,
    uc_mem_type  type,
    uint64_t     address,
    int          size,
    int64_t      value,
    void        *user_data
    )
{
    MemoryRegion *region;

    assert(user_data != NULL);
    region = (MemoryRegion *)user_data;

    if (region->m_handler == NULL) {
        return;
    }

    MemoryRegionEvent event = {
        .addr = address,
        .size = size,
        .value = value,
    };
    
    switch (type) {
    case UC_MEM_READ:  event.type = MEM_EVENT_READ; break;
    case UC_MEM_WRITE: event.type = MEM_EVENT_WRITE; break;
    default:           assert(0); break;
    }

    region->m_handler(region, &event, region->m_handler_user);
}

/*!
 * Map the regions of memory into the address space
 */
int UnicornCpu::MemMap(MemoryRegion *mem)
{
    uc_err err;

    // FIXME: We should flatten out the address space to handle sub-sub regions

    assert(mem->m_start == 0);

    for (int i = 0; i < mem->m_subregions.size(); i++) {
        log_debug("Mapping Region %d [%08x-%08zx)\n", i, mem->m_subregions[i]->m_start, mem->m_subregions[i]->m_start + mem->m_subregions[i]->m_size);

        err = uc_mem_map_ptr(m_uc,
                             mem->m_subregions[i]->m_start,
                             mem->m_subregions[i]->m_size,
                             UC_PROT_READ | UC_PROT_WRITE | UC_PROT_EXEC,
                             mem->m_subregions[i]->m_data);

        if (err != UC_ERR_OK) {
            log_error("uc_mem_map_ptr failed (%u)\n", err);
            return -1;
        }

        // Install hook for this region
        if (mem->m_subregions[i]->m_handler != NULL) {
            uc_hook hook;
            uint64_t start, limit;
            start = mem->m_subregions[i]->m_start;
            limit = start + mem->m_subregions[i]->m_size - 1;
            log_debug("\tAdding hook!\n");
            err = uc_hook_add(m_uc, &hook, // FIXME: We leak these hooks! They should be cleaned up.
                              UC_HOOK_MEM_VALID, (void*)xb_uc_hook,
                              mem->m_subregions[i], start, limit);
            if (err != UC_ERR_OK) {
                log_error("uc_hook_add failed (%u)\n", err);
                return -1;
            }
        }
    }
    return 0;
}
