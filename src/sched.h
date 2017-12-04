#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "cpu.h"
#include <string.h>
#include <vector>

/*!
 * Emulated thread
 * 
 * This class encapsulates a basic thread structure for the Xbox CPU.
 */
class Thread {
public:
    uint32_t   m_entry;
    uint32_t   m_stack_base;
    size_t     m_stack_size;
    CpuContext m_context;

    Thread(uint32_t entry, uint32_t stack_base, size_t stack_size);
    ~Thread();
};

#define SCHEDULER_EXIT_ERROR  (-1)
#define SCHEDULER_EXIT_HLT    0
#define SCHEDULER_EXIT_EXPIRE 1

/*!
 * CPU Scheduler
 * 
 * Responsible for switching the CPU between the various execution threads.
 */
class Scheduler {
protected:
    Cpu                   *m_cpu;
    std::vector<Thread *>  m_threads;

public:
    Scheduler(Cpu *cpu);
    ~Scheduler();
    int ScheduleThread(Thread *thread);
    int Run();
};

#endif
