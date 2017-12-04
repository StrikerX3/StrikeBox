#include "sched.h"
#include "log.h"

/*!
 * Constructor
 */
Thread::Thread(uint32_t entry, uint32_t stack_base, size_t stack_size)
: m_entry(entry), m_stack_base(stack_base), m_stack_size(stack_size)
{
    m_context.m_regs[REG_EIP] = m_entry;
    m_context.m_regs[REG_ESP] = m_stack_base + m_stack_size;
}

/*!
 * Destructor
 */
Thread::~Thread()
{
}

/*!
 * Constructor
 */
Scheduler::Scheduler(Cpu *cpu)
{
    m_cpu = cpu;
}

/*!
 * Destructor
 */
Scheduler::~Scheduler()
{
}

/*!
 * Schedule a thread for execution
 */
int Scheduler::ScheduleThread(Thread *thread)
{
    m_threads.push_back(thread);

    // FIXME: Move this out of here after we support concurrency
    m_cpu->RegWrite(REG_ESP, thread->m_stack_base + thread->m_stack_size);
    m_cpu->RegWrite(REG_EIP, thread->m_entry);

    return 0;
}

/*!
 * Begin CPU execution
 */
int Scheduler::Run()
{
    uint32_t reg;
    int result;

    // Get current instruction pointer
    m_cpu->RegRead(REG_EIP, &reg);
    log_debug("CPU starting at %x\n", reg);

    // Allow the CPU to execute for a fixed amount of time (or shorter if the
    // guest code causes a VM exit)
    result = m_cpu->Run(100*1000); // 100ms quantum
    if (result) {
        return SCHEDULER_EXIT_ERROR;
    }

    // Read the exit instruction pointer
    m_cpu->RegRead(REG_EIP, &reg);
    log_debug("CPU stopped at %x (reason %d)\n", reg, result);

    // Debugging
    m_cpu->PrintRegisters();

    return 0;
}
