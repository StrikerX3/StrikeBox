#include <assert.h>
#include <stdio.h>
#include "cpu.h"
#include "log.h"

/*!
 * Initialize a CpuContext object
 */
CpuContext::CpuContext()
{
    for (int i = 0; i < REG_MAX; i++) {
        m_regs[i] = 0;
    }
}

/*!
 * Destructor
 */
CpuContext::~CpuContext()
{
}

/*!
 * Destructor
 */
Cpu::~Cpu()
{
}

/*!
 * Print the CPU registers and portion of the stack
 */
void Cpu::PrintRegisters()
{
    uint32_t value;
    const char *blank = "                ";

    RegRead(REG_EIP,    &value); log_debug("EIP = %08x  ",    value);
    RegRead(REG_EFLAGS, &value); log_debug("EFLAGS = %08x\n", value);
    RegRead(REG_CR0,    &value); log_debug("CR0 = %08x  ",    value);
    RegRead(REG_CS,     &value); log_debug("CS = %08x  ",     value);
    RegRead(REG_EAX,    &value); log_debug("EAX = %08x  ",    value);
    RegRead(REG_ESP,    &value); log_debug("ESP = %08x\n",    value);
    RegRead(REG_CR1,    &value); log_debug("CR1 = %08x  ",    value);
    RegRead(REG_SS,     &value); log_debug("SS = %08x  ",     value);
    RegRead(REG_ECX,    &value); log_debug("ECX = %08x  ",    value);
    RegRead(REG_EBP,    &value); log_debug("EBP = %08x\n",    value);
    RegRead(REG_CR2,    &value); log_debug("CR2 = %08x  ",    value);
    RegRead(REG_DS,     &value); log_debug("DS = %08x  ",     value);
    RegRead(REG_EDX,    &value); log_debug("EDX = %08x\n",    value);
    RegRead(REG_CR3,    &value); log_debug("CR3 = %08x  ",    value);
    RegRead(REG_ES,     &value); log_debug("ES = %08x  ",     value);
    RegRead(REG_EBX,    &value); log_debug("EBX = %08x\n",    value);
    RegRead(REG_CR4,    &value); log_debug("CR4 = %08x  ",    value);
    RegRead(REG_FS,     &value); log_debug("FS = %08x  ",     value);
    RegRead(REG_ESI,    &value); log_debug("ESI = %08x\n",    value);
    log_debug("%s", blank);
    RegRead(REG_GS,     &value); log_debug("GS = %08x  ",     value);
    RegRead(REG_EDI,    &value); log_debug("EDI = %08x\n",    value);

    // FIXME
    // RegRead(REG_IDT_BASE, &value);    log_debug("IDT_BASE = %08x\n", value);
    // RegRead(REG_IDT_LIMIT, &value);    log_debug("IDT_LIMIT = %08x\n", value);
    // RegRead(REG_GDT_BASE, &value);    log_debug("GDT_BASE = %08x\n", value);
    // RegRead(REG_GDT_LIMIT, &value);    log_debug("GDT_LIMIT = %08x\n", value);

    uint32_t esp;
    RegRead(REG_ESP, &esp);
    for (int i = 10; i >= 0; i--) {
        uint32_t addr, val;
        addr = esp + i * 4;
        MemRead(addr, 4, &val);
        log_debug("%08x: %08x\n", addr, val);
    }
}

/*!
 * Save the CPU context into the supplied context
 */
int Cpu::SaveContext(CpuContext *context)
{
    for (int i = 0; i < REG_MAX; i++) {
        RegRead((enum CpuReg)i, &context->m_regs[i]);
    }

    return 0;
}

/*!
 * Restore the supplied context
 */
int Cpu::RestoreContext(CpuContext *context)
{
    for (int i = 0; i < REG_MAX; i++) {
        RegWrite((enum CpuReg)i, context->m_regs[i]);
    }

    return 0;
}

/*!
 * Basic calling convention helper object initialization
 */
CallingConventionHelper::CallingConventionHelper(Cpu *cpu)
{
    m_cpu = cpu;
    m_arg_count = 0;
    m_stack_cleanup_size = sizeof(uint32_t); // Always pop the return address
    m_cpu->RegRead(REG_ESP, &m_esp);
    m_cpu->MemRead(m_esp, sizeof(uint32_t), &m_return_addr);
}

/*!
 * Set the value which should be returned according to the convention
 */
void CallingConventionHelper::SetReturnValue(void *val, size_t size)
{
    uint32_t reg = 0;

    if (size == 1) {
        reg = *(uint8_t *)val;
    } else if (size == 2) {
        reg = *(uint16_t *)val;
    } else if (size == 4) {
        reg = *(uint32_t *)val;
    } else {
        assert(0); // FIXME
    }

    m_cpu->RegWrite(REG_EAX, reg);
}

/*!
 * Cleanup the stack before returning to the calling function
 */
void CallingConventionHelper::Cleanup()
{
    log_debug("Cleaning up %zd bytes from the stack\n", m_stack_cleanup_size);
    m_cpu->RegWrite(REG_ESP, m_esp + m_stack_cleanup_size);
}

/*!
 * Get the return adress
 */
uint32_t CallingConventionHelper::GetReturnAddress()
{
    return m_return_addr;
}

/*!
 * Constructor
 */
CdeclHelper::CdeclHelper(Cpu *cpu)
: CallingConventionHelper(cpu)
{
}

/*!
 * Get the next argument according to the calling convention
 */
void CdeclHelper::GetArgument(void *val, size_t size)
{
    uint32_t reg;

    assert(size <= 4); // FIXME: Handle structs and larger values

    // Value is stored on stack
    m_cpu->MemRead(m_esp + 4 + 4*(m_arg_count), 4, &reg);

    if (size == 1) {
        *(uint8_t *)val = (uint8_t)reg;
    } else if (size == 2) {
        *(uint16_t *)val = (uint16_t)reg;
    } else if (size == 4) {
        *(uint32_t *)val = (uint32_t)reg;
    } else {
        assert(0); // FIXME
    }

    m_arg_count += 1;
}

/*!
 * Constructor
 */
StdcallHelper::StdcallHelper(Cpu *cpu)
: CallingConventionHelper(cpu)
{
}

/*!
 * Get the next argument according to the calling convention
 */
void StdcallHelper::GetArgument(void *val, size_t size)
{
    uint32_t reg;

    assert(size <= 4); // FIXME: Handle structs and larger values

    // Value is stored on stack
    m_cpu->MemRead(m_esp + 4 + 4*(m_arg_count), 4, &reg);
    m_stack_cleanup_size += 4;

    if (size == 1) {
        *(uint8_t *)val = (uint8_t)reg;
    } else if (size == 2) {
        *(uint16_t *)val = (uint16_t)reg;
    } else if (size == 4) {
        *(uint32_t *)val = (uint32_t)reg;
    } else {
        assert(0); // FIXME
    }

    m_arg_count += 1;
}

/*!
 * Constructor
 */
FastcallHelper::FastcallHelper(Cpu *cpu)
: CallingConventionHelper(cpu)
{
    // Keep track of how many arguments we have passed through registers
    m_arg_pass_thru_reg_count = 0;
}

/*!
 * Get the next argument according to the calling convention
 */
void FastcallHelper::GetArgument(void *val, size_t size)
{
    uint32_t reg;

    assert(size <= 4); // FIXME: Handle structs and larger values

    if (m_arg_pass_thru_reg_count == 0) {
        // Value is stored in register ECX
        m_cpu->RegRead(REG_ECX, &reg);
        m_arg_pass_thru_reg_count += 1;
    } else if (m_arg_pass_thru_reg_count == 1) {
        // Value is stored in register EDX
        m_cpu->RegRead(REG_EDX, &reg);
        m_arg_pass_thru_reg_count += 1;
    } else {
        // Value is stored on stack
        m_cpu->MemRead(m_esp + 4 + 4*(m_arg_count - m_arg_pass_thru_reg_count), 4, &reg);
        m_stack_cleanup_size += 4;
    }

    if (size == 1) {
        *(uint8_t *)val = (uint8_t)reg;
    } else if (size == 2) {
        *(uint16_t *)val = (uint16_t)reg;
    } else if (size == 4) {
        *(uint32_t *)val = (uint32_t)reg;
    } else {
        assert(0); // FIXME
    }

    m_arg_count += 1;
}
