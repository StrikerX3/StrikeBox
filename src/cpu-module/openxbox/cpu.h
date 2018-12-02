#pragma once

#include <stdint.h>
#include <string.h>
#include <vector>
#include <queue>
#include <mutex>

#include "openxbox/memregion.h"
#include "openxbox/gdt.h"
#include "openxbox/idt.h"
#include "openxbox/io.h"
#include "openxbox/bitmap.h"

namespace openxbox {

// These constants control how often the CPU can handle interrupts, while
// giving some affordance to handle short bursts.
// Without this, the emulator might get stuck processing interrupts alone.
#ifdef _DEBUG
static const uint8_t kInterruptHandlerMaxCredits = 25;   // Maximum amount of credits available to handle interrupts
static const uint8_t kInterruptHandlerCost = 5;          // Credits spent when an interrupt is handled
static const uint8_t kInterruptHandlerIncrement = 1;     // Credits recovered when CPU emulation starts
#else
static const uint8_t kInterruptHandlerMaxCredits = 200;  // Maximum amount of credits available to handle interrupts
static const uint8_t kInterruptHandlerCost = 2;          // Credits spent when an interrupt is handled
static const uint8_t kInterruptHandlerIncrement = 1;     // Credits recovered when CPU emulation starts
#endif


// CR0 bits
#define CR0_PG       0x80000000  // paging
#define CR0_CD       0x40000000  // cache disable
#define CR0_NW       0x20000000  // not write-through
#define CR0_AM       0x00040000  // alignment mask
#define CR0_WP       0x00010000  // write protect
#define CR0_NE       0x00000020  // numeric error
#define CR0_ET       0x00000010  // extension type (80387)
#define CR0_TS       0x00000008  // task switched
#define CR0_EM       0x00000004  // emulate math coprocessor
#define CR0_MP       0x00000002  // math present
#define CR0_PE       0x00000001  // protection enable

// CR4 bits
#define CR4_VME      0x00000001  // V86 mode extensions
#define CR4_PVI      0x00000002  // Protected mode virtual interrupts
#define CR4_TSD      0x00000004  // Time stamp disable
#define CR4_DE       0x00000008  // Debugging Extensions
#define CR4_PSE      0x00000010  // Page size extensions
#define CR4_PAE      0x00000020  // Physical address extensions
#define CR4_MCE      0x00000040  // Machine check enable
#define CR4_PGE      0x00000080  // Page global enable
#define CR4_FXSR     0x00000200  // FXSR used by OS
#define CR4_XMMEXCPT 0x00000400  // XMMI used by OS

// EFLAGS
#define CF_BIT      0              // Carry flag
#define CF_MASK     (1 << CF_BIT)
#define PF_BIT      2              // Parity flag
#define PF_MASK     (1 << PF_BIT)
#define AF_BIT      4              // Adjust flag
#define AF_MASK     (1 << AF_BIT)
#define ZF_BIT      6              // Zero flag
#define ZF_MASK     (1 << ZF_BIT)
#define SF_BIT      7              // Sign flag
#define SF_MASK     (1 << SF_BIT)
#define TF_BIT      8              // Trap flag
#define TF_MASK     (1 << TF_BIT)
#define IF_BIT      9              // Interrupt flag
#define IF_MASK     (1 << IF_BIT)
#define DF_BIT      10             // Direction flag
#define DF_MASK     (1 << DF_BIT)
#define OF_BIT      11             // Overflow flag
#define OF_MASK     (1 << OF_BIT)
#define IOPL_MASK   0x3000         // IO privilege level
#define IOPL_BIT0   12
#define IOPL_BIT1   13
#define NT_BIT      14             // Nested task
#define NT_MASK     (1 << NT_BIT)
#define RF_BIT      16             // Resume flag
#define RF_MASK     (1 << RF_BIT)
#define VM_BIT      17             // Virtual Mode
#define VM_MASK     (1 << VM_BIT)
#define AC_BIT      18             // Alignment check
#define AC_MASK     (1 << AC_BIT)
#define VIF_BIT     19             // Virtual Interrupt flag
#define VIF_MASK    (1 << VIF_BIT)
#define VIP_BIT     20             // Virtual Interrupt pending
#define VIP_MASK    (1 << VIP_BIT)


/*!
 * Enumeration type for standard registers
 */
enum CpuReg {
	REG_EIP,
	REG_EFLAGS,
	REG_EAX,
	REG_ECX,
	REG_EDX,
	REG_EBX,
	REG_ESI,
	REG_EDI,
	REG_ESP,
	REG_EBP,
	REG_CS,
	REG_SS,
	REG_DS,
	REG_ES,
	REG_FS,
	REG_GS,
	REG_TR,
	REG_CR0,
	REG_CR2,
	REG_CR3,
	REG_CR4,
	// TODO: add floating-point, MMX and SSE registers
	REG_MAX,
};

// Last register to save in CpuContext
#define REG_CONTEXT_MAX   ((size_t)(REG_GDT_LIMIT + 1))

/*!
 * Exit Info
 */
enum CpuExitReason {
	CPU_EXIT_NORMAL,         // Time slice expiration
	CPU_EXIT_ERROR,          // Non-specific error
	CPU_EXIT_INTERRUPT,      // Interrupt
	CPU_EXIT_HLT,            // HLT instruction
    CPU_EXIT_SHUTDOWN,       // System shutdown
    CPU_EXIT_SW_BREAKPOINT,  // Software breakpoint
    CPU_EXIT_HW_BREAKPOINT,  // Hardware breakpoint
};

enum InterruptResult {
	INTR_SUCCESS,      // The interrupt was successfully delivered to the CPU
	INTR_DISABLED,     // The interrupt was not delivered because interrupts are disabled
	INTR_MASKED,       // The interrupt was masked by the CPU
	INTR_PENDING,      // There is an unhandled pending interrupt from a previous interrupt call
	INTR_FAILED,       // Interrupt handling failed
	INTR_NO_HANDLER,   // No interrupt handler was specified
};

struct CpuExitInfo {
	enum CpuExitReason reason;
	uint8_t            intr_vector;
};

typedef void (*InterruptHandlerFunc)(uint8_t vector, void *data);

struct PhysicalMemoryRange {
    char *data;
    uint32_t startingAddress;
    uint32_t endingAddress;
};

enum HardwareBreakpointTrigger {
    HWBP_TRIGGER_EXECUTION = 0,
    HWBP_TRIGGER_DATA_WRITE,
    HWBP_TRIGGER_8_BYTE_WIDE,
    HWBP_TRIGGER_DATA_READ_WRITE,
};

enum HardwareBreakpointLength {
    HWBP_LENGTH_1_BYTE = 0,
    HWBP_LENGTH_2_BYTE,
    HWBP_LENGTH_8_BYTE,
    HWBP_LENGTH_4_BYTE,
};

struct HardwareBreakpoints {
    struct {
        uint64_t address;
        bool localEnable;
        bool globalEnable;
        HardwareBreakpointTrigger trigger;
        HardwareBreakpointLength length;
    } bp[4];
};

/*!
 * CPU base class
 *
 * Defines a basic interface for the CPU that will be subclassed by actual
 * implementations.
 */
class Cpu {
public:
	// Constructor.
	Cpu();

	// Destructor.
	virtual ~Cpu();

	// ----- Basic CPU operations ---------------------------------------------

	/*!
	 * Initializes the CPU.
	 */
	int Initialize(IOMapper *ioMapper);

	/*!
	 * Runs the CPU until interrupted.
	 */
	int Run();

	/*!
	 * Runs one instruction on the CPU.
	 */
	int Step();
	
	/*!
	 * Sends an interrupt to the CPU, optionally making it non maskable.
	 *
	 * If interrupts are disabled, returns INTR_DISABLED.
	 * If the interrupt was masked, returns INTR_MASKED.
	 * Otherwise it enqueues the interrupt request, stops CPU emulation and
	 * returns INTR_SUCCESS.
	 */
	InterruptResult Interrupt(uint8_t vector);

	// ----- Physical memory --------------------------------------------------

	/*!
	 * Maps memory regions, including optional memory access handlers.
	 */
	int MemMap(MemoryRegion *mem);

	/*!
	 * Maps a memory subregion.
	 */
	virtual int MemMapSubregion(MemoryRegion *subregion) = 0;

	/*!
	 * Reads a portion of physical memory into the specified value.
	 */
	int MemRead(uint32_t addr, uint32_t size, void *value);

	/*!
	 * Writes the specified value into physical memory.
	 */
	int MemWrite(uint32_t addr, uint32_t size, void *value);

	// ----- Virtual memory ---------------------------------------------------

	/*!
	 * Maps a virtual address to a physical address.
	 */
	bool VirtualToPhysical(uint32_t vaddr, uint32_t *paddr);

	/*!
	 * Reads a portion of virtual memory into the specified value. x86 virtual
	 * address translation is performed based on the current registers and
	 * memory contents.
	 */
	int VMemRead(uint32_t vaddr, uint32_t size, void *value);

	/*!
	 * Writes the specified value into virtual memory. x86 virtual address
	 * translation is performed based on the current registers and memory
	 * contents.
	 */
	int VMemWrite(uint32_t vaddr, uint32_t size, void *value);
	
	// ----- Stack ------------------------------------------------------------

	/*!
	 * Creates space in the stack by subtracting the size from ESP.
	 */
	int CreateStackSpace(uint32_t size);

	/*
	 * Reclaims space in the stack by adding the size to ESP.
	 */
	int ReclaimStackSpace(uint32_t size);

	// ----- Registers --------------------------------------------------------
	
	/*!
	 * Reads from a register.
	 */
	virtual int RegRead(enum CpuReg reg, uint32_t *value) = 0;

	/*!
	 * Writes to a register.
	 */
	virtual int RegWrite(enum CpuReg reg, uint32_t value) = 0;

	/*!
	 * Copies the value from the source register to the destinatioon register.
	 */
	int RegCopy(enum CpuReg dst, enum CpuReg src);

	/*!
	 * Gets the Global Descriptor Table.
	 */
	virtual int GetGDT(uint32_t *base, uint32_t *limit) = 0;

	/*!
	 * Sets the Global Descriptor Table.
	 */
	virtual int SetGDT(uint32_t base, uint32_t limit) = 0;

	/*!
	 * Retrieves an entry in the Global Descriptor Table.
	 * Returns zero on success, non-zero if the index is out of bounds.
	 */
	int GetGDTEntry(uint16_t selector, GDTEntry *entry);

	/*!
	 * Modifies an entry in the Global Descriptor Table.
	 * Returns zero on success, non-zero if the index is out of bounds.
	 */
	int SetGDTEntry(uint16_t selector, GDTEntry *entry);

	/*!
	 * Gets the Interrupt Descriptor Table.
	 */
	virtual int GetIDT(uint32_t *base, uint32_t *limit) = 0;

	/*!
	 * Sets the Interrupt Descriptor Table.
	 */
	virtual int SetIDT(uint32_t base, uint32_t limit) = 0;

	/*!
	 * Retrieves an entry in the Interrupt Descriptor Table.
	 * Returns zero on success, non-zero if the index is out of bounds.
	 */
	int GetIDTEntry(uint8_t vector, IDTEntry *entry);
	
	/*!
	 * Modifies an entry in the Interrupt Descriptor Table.
	 * Returns zero on success, non-zero if the index is out of bounds.
	 */
	int SetIDTEntry(uint8_t vector, IDTEntry *entry);

	/*!
	 * Enables or disables interrupts by changing the IF flag of the EFLAGS
	 * register.
	 *
	 * Equivalent to the `cli` and `sti` instructions.
	 */
	void SetInterruptsEnabled(bool enabled);

	/*!
	 * Sets the specified bits of the EFLAGS register.
	 */
	int SetFlags(uint32_t flagsBits);

	/*!
	 * Clears the specified bits of the EFLAGS register.
	 */
	int ClearFlags(uint32_t flagsBits);

	// ----- Instructions -----------------------------------------------------

	/*!
	 * Pushes an immediate 32-bit value onto the stack.
	 *
	 * Equivalent to "push <value>".
	 */
	int Push(uint32_t value);

	/*!
	 * Pushes a CPU register onto the stack.
	 *
	 * Equivalent to "push <reg>".
	 */
	int PushReg(enum CpuReg reg);

	/*!
	 * Pushes the CPU's flags onto the stack.
	 *
	 * Equivalent to "pushfd".
	 */
	int PushFlags();

	/*!
	 * Pops an immediate 32-bit value from the stack.
	 *
	 * Equivalent to "pop", except the value is written to the specified variable.
	 */
	int Pop(uint32_t *value);

	/*!
	 * Pops a value from the stack into the specified CPU register.
	 *
	 * Equivalent to "pop <reg>".
	 */
	int PopReg(enum CpuReg reg);

	/*!
	 * Pops the CPU's flags from the stack.
	 *
	 * Equivalent to "popfd".
	 */
	int PopFlags();

	/*!
	 * Returns to the address at the top of the stack.
	 *
	 * Equivalent to "ret", or "pop eip" (if it was possible).
	 */
	int Ret();

    // ----- Breakpoints ------------------------------------------------------

    /*!
     * Enables or disables software breakpoints.
     *
     * If the guest executes an INT 3h instruction while software breakpoints are enabled,
     * the CPU emulator will stop and return CPU_EXIT_BREAKPOINT. The guest code will not
     * have a chance to handle INT 3h while software breakpoints are enabled.
     *
     * This is an optional operation. TODO: implement capability checking.
     */
    virtual int EnableSoftwareBreakpoints(bool enable);

    /*!
     * Configures up to 4 hardware breakpoints.
     *
     * While these breakpoints are active, when the guest code triggers them, the CPU
     * emulator will stop and return CPU_EXIT_BREAKPOINT.
     *
     * This is an optional operation. TODO: implement capability checking.
     */
    virtual int SetHardwareBreakpoints(HardwareBreakpoints breakpoints);

    /*!
     * Clears all hardware breakpoints.
     *
     * This is an optional operation. TODO: implement capability checking.
     */
    virtual int ClearHardwareBreakpoints();

    /*!
     * Retrieves the address of the most recently hit breakpoint. Must be invoked
     * after the CPU emulator stops due to a breakpoint, and before running the CPU again.
     * Returns false if a breakpoint was not hit as of the most recent execution.
     *
     * This is an optional operation. TODO: implement capability checking.
     */
    virtual bool GetBreakpointAddress(uint32_t *address);

	// ----- Data -------------------------------------------------------------

	/*!
	 * Retrieves information about why the CPU emulation exited.
	 */
	struct CpuExitInfo* GetExitInfo() { return &m_exitInfo; }
protected:
	/*!
	 * The CPU exit information.
	 */
	struct CpuExitInfo m_exitInfo;

    /*!
     * The I/O mapper that handles I/O and MMIO for the CPU.
     */
    IOMapper *m_ioMapper;

	/*!
	 * Allows the implementation to do further initialization.
	 */
	virtual int InitializeImpl() = 0;

	/*!
	 * Runs the CPU until interrupted.
	 */
	virtual int RunImpl() = 0;

	/*!
	 * Runs one instruction on the CPU.
	 */
	virtual int StepImpl() = 0;

	/*!
	 * Sends an interrupt to the CPU.
	 */
	virtual InterruptResult InterruptImpl(uint8_t vector) = 0;
	
    /*!
     * Injects an interrupt into the VCPU.
     */
    virtual int InjectInterrupt(uint8_t vector) = 0;

    /*!
     * Determines if an interrupt can be injected into the VCPU.
     */
    virtual bool CanInjectInterrupt() = 0;

    /*!
     * Requests for an interrupt injection window.
     */
    virtual void RequestInterruptWindow() = 0;

private:
    // TODO: use an AVL tree instead of a vector to speed up lookups
    std::vector<PhysicalMemoryRange *> m_physMemMap;

    std::mutex m_interruptMutex;
    std::mutex m_pendingInterruptsMutex;
    std::queue<uint8_t> m_pendingInterrupts;
    uint8_t m_interruptHandlerCredits;

    void InjectPendingInterrupt();
};

}
