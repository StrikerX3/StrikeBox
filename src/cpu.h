#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <string.h>
#include "mem.h"

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
    REG_IDT_BASE,
    REG_IDT_LIMIT,
    REG_GDT_BASE,
    REG_GDT_LIMIT,
    REG_CR0,
    REG_CR1,
    REG_CR2,
    REG_CR3,
    REG_CR4,
    REG_MAX,
};

/*!
 * CPU execution context
 * 
 * This class can be used to save and restore the state of the CPU.
 */
class CpuContext {
public:
    uint32_t m_regs[REG_MAX];
    CpuContext();
    ~CpuContext();
};

/*!
 * Exit Info
 */
enum CpuExitReason {
    EXIT_NORMAL,    // Exit due to time slice expiration or HLT
    EXIT_ERROR,     // Exit due to non-specific error
    EXIT_INTERRUPT, // Exit due to interrupt
};

struct CpuExitInfo {
    enum CpuExitReason reason;
    int                intr_vector;
};

/*!
 * CPU base class
 *
 * Defines a basic interface for the CPU that will be subclassed by actual
 * implementations.
 */
class Cpu {
public:
    virtual ~Cpu();
    virtual int Initialize() = 0;
    virtual int MemMap(MemoryRegion *mem) = 0;
    virtual int MemRead(uint32_t addr, size_t size, void *value) = 0;
    virtual int MemWrite(uint32_t addr, size_t size, void *value) = 0;
    virtual int RegRead(enum CpuReg reg, uint32_t *value) = 0;
    virtual int RegWrite(enum CpuReg reg, uint32_t value) = 0;
    virtual int Run(uint64_t time_limit_us) = 0;
    virtual int SaveContext(CpuContext *context);
    virtual int RestoreContext(CpuContext *context);
    virtual struct CpuExitInfo *GetExitInfo() = 0;
    void PrintRegisters();
};

/*!
 * Supported calling conventions
 */
enum CallingConventionType {
	CALLING_CONVENTION_INVALID = -1,
	CALLING_CONVENTION_CDECL = 0,
	CALLING_CONVENTION_STDCALL = 1,
	CALLING_CONVENTION_FASTCALL = 2,
	CALLING_CONVENTION_MAX
};

/*!
 * Calling convention helper base class
 * 
 * The calling convention helper objects are used to help retrieve arguments
 * from registers/stack when handling a function call outside of the CPU
 * emulation. Specifically, these are used when emulating the Kernel functions.
 */
class CallingConventionHelper {
protected:
    Cpu      *m_cpu;
    size_t    m_arg_count;
    size_t    m_stack_cleanup_size;
    uint32_t  m_return_addr;
    uint32_t  m_esp;

public:
    CallingConventionHelper(Cpu *cpu);
	virtual void GetArgument(void *val, size_t size) = 0;
	virtual void SetReturnValue(void *val, size_t size);
    virtual uint32_t GetReturnAddress();
    virtual void Cleanup();
};

/*!
 * CDECL calling convention helper
 * 
 * This is the helper class for the "cdecl" calling convention; that is, the
 * standard C calling convention whereby arguments are passed right-to-left
 * on the stack, and the calling function is responsible for removing the
 * arguments. This convention is used when functions take a variable number of
 * arguments (e.g. printf).
 */
class CdeclHelper : public CallingConventionHelper {
public:
    CdeclHelper(Cpu *cpu);
	void GetArgument(void *val, size_t size);
};

/*!
 * STDCALL calling convention helper
 * 
 * This is the helper class for the "stdcall" calling convention, wherein the
 * arguments are passed on the stack as with the CDECL convention, but the
 * function being called must remove the arguments from the stack.
 */
class StdcallHelper : public CallingConventionHelper {
public:
    StdcallHelper(Cpu *cpu);
	void GetArgument(void *val, size_t size);
};

/*!
 * FASTCALL calling convention helper
 * 
 * This is the helper class for the "fastcall" calling convention, wherein
 * argument values are passed through both registers and the stack. The first
 * two arguments of size <= 4 will be provided in registers ECX and EDX,
 * respectively. Other arguments will be provided on stack and must be removed
 * by callee. Return value is in EAX.
 */
class FastcallHelper : public CallingConventionHelper {
protected:
    size_t m_arg_pass_thru_reg_count;

public:
    FastcallHelper(Cpu *cpu);
	void GetArgument(void *val, size_t size);
};

#endif
