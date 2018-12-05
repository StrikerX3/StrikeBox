// Bus Master PCI IDE Controller emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// Based on the Programming Interface for Bus Master IDE Controller
// Revision 1.0 available at http://www.bswd.com/idems100.pdf
// and complemented with information from PC87415 at
// https://parisc.wiki.kernel.org/images-parisc/0/0a/PC87415.pdf
#include "bmide.h"
#include "vixen/log.h"
#include "vixen/thread.h"

namespace vixen {

using namespace hw::bmide;

BMIDEChannel::BMIDEChannel(Channel channel)
    : m_channel(channel)
{
    m_worker_running = true;
    m_job_running = false;
    m_workerThread = std::thread(WorkerThreadFunc, this);
}

BMIDEChannel::~BMIDEChannel() {
    if (m_workerThread.joinable()) {
        // Tell the worker to stop running and notify it immediately
        m_worker_running = false;
        m_job_running = false;
        m_jobCond.notify_one();

        // Wait for the worker thread to stop
        m_workerThread.join();
    }
}

void BMIDEChannel::ReadCommand(uint32_t *value, uint8_t size) {
    *value = m_command & kCommandRegMask;
}

void BMIDEChannel::ReadStatus(uint32_t *value, uint8_t size) {
    *value = m_status & kStatusRegMask;
}

void BMIDEChannel::ReadPRDTableAddress(uint32_t *value, uint8_t size) {
    if (size == 1) {
        *(uint8_t*)value = m_prdTableAddrs;
    }
    else if (size == 2) {
        *(uint16_t*)value = m_prdTableAddrs;
    }
    else {
        *value = m_prdTableAddrs;
    }
}

void BMIDEChannel::WriteCommand(uint32_t value, uint8_t size) {
    m_command = value;
    if (value & CmdStartStopBusMaster) {
        BeginWork();
    }
    else {
        StopWork();
    }
}

void BMIDEChannel::WriteStatus(uint32_t value, uint8_t size) {
    // Clear interrupt and error flags if requested
    m_status &= ~(value & kStatusRegWriteClearMask);

    // Update writable bits
    m_status &= ~kStatusRegWriteMask;
    m_status |= value & kStatusRegWriteMask;
}

void BMIDEChannel::WritePRDTableAddress(uint32_t value, uint8_t size) {
    // Clear least significant bit, which must always be zero
    value &= ~1;

    // Check for unaligned address
    if (value & (sizeof(uint32_t) - 1)) {
        log_warning("BMIDEChannel::WritePRDTableAddress:  Guest wrote unaligned PRD table address: 0x%x\n", value);
    }

    // Update register value
    if (size == 1) {
        m_prdTableAddrs = (uint8_t)value;
    }
    else if (size == 2) {
        m_prdTableAddrs = (uint16_t)value;
    }
    else {
        m_prdTableAddrs = value;
    }
    log_spew("BMIDEChannel::WritePRDTableAddress:  channel = %d,  address = 0x%x\n", m_channel, m_prdTableAddrs);
}

void BMIDEChannel::BeginWork() {
    if (m_job_running) {
        log_warning("BMIDEChannel::BeginWork:  Attempted to start operation on channel = %d while an operation is already in progress!\n", m_channel);
        return;
    }

    log_spew("BMIDEChannel::BeginWork:  Starting operation on channel %d\n", m_channel);

    // Prepare job and notify worker
    m_job_running = true;
    m_jobCond.notify_one();
}

void BMIDEChannel::StopWork() {
    if (!m_job_running) {
        log_warning("BMIDEChannel::StopWork:  Attempted to stop operation on channel %d while no operation is in progress!\n", m_channel);
    }

    log_spew("BMIDEChannel::StopWork:  Stopping operation on channel = %d\n", m_channel);

    // Tell worker to stop working on the job
    m_job_running = false;
}

// Worker thread function
uint32_t BMIDEChannel::WorkerThreadFunc(void *data) {
    BMIDEChannel *chan = (BMIDEChannel *)data;
    
    char threadName[37];
    sprintf(threadName, "[HW] Bus Master IDE Channel %d Worker", chan->m_channel);
    Thread_SetName(threadName);
    
    chan->RunWorker();
    return 0;
}

void BMIDEChannel::RunWorker() {
    while (m_worker_running) {
        // Wait for work
        {
            std::unique_lock<std::mutex> lock(m_jobMutex);
            m_jobCond.wait(lock);
        }

        // Do work
        while (m_job_running) {
            bool isWrite = (m_command & CmdReadWriteControl) != 0;
            log_spew("BMIDEChannel %d: %s\n", m_channel, (isWrite ? "write" : "read"));
            // TODO: implement
            m_job_running = false;
        }
    }
}

}
