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
using namespace hw::ata;

BMIDEChannel::BMIDEChannel(Channel channel, ATAChannel& ataChannel, uint8_t *ram, uint32_t ramSize)
    : m_channel(channel)
    , m_ataChannel(ataChannel)
    , m_ram(ram)
    , m_ramSize(ramSize) 
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
        *(uint8_t*)value = m_prdTableAddr;
    }
    else if (size == 2) {
        *(uint16_t*)value = m_prdTableAddr;
    }
    else {
        *value = m_prdTableAddr;
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
        m_prdTableAddr = (uint8_t)value;
    }
    else if (size == 2) {
        m_prdTableAddr = (uint16_t)value;
    }
    else {
        m_prdTableAddr = value;
    }
    log_spew("BMIDEChannel::WritePRDTableAddress:  channel = %d,  address = 0x%x\n", m_channel, m_prdTableAddr);
}

void BMIDEChannel::BeginWork() {
    log_spew("BMIDEChannel::BeginWork:  Starting operation on channel %d\n", m_channel);

    m_status |= StActive;

    // Prepare job and notify worker
    m_job_running = true;
    m_job_cancel = false;
    m_jobCond.notify_one();
}

void BMIDEChannel::StopWork() {
    log_spew("BMIDEChannel::StopWork:  Stopping operation on channel = %d\n", m_channel);

    // Interrupt worker
    m_job_cancel = true;
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

static struct PRDHelper {
    uint8_t *m_ram;
    uint32_t m_ramSize;
    PhysicalRegionDescriptor *m_currPRD;
    uint32_t m_currByte;
    uint32_t physAddr;
    uint8_t *bufPtr;

    PRDHelper(uint8_t *ram, uint32_t ramSize, uint32_t prdTableAddress)
        : m_ram(ram)
        , m_ramSize(ramSize)
    {
        // TODO: validate against ramSize to prevent buffer overflows
        m_currPRD = reinterpret_cast<PhysicalRegionDescriptor*>(m_ram + prdTableAddress);
        m_currByte = 0;
        physAddr = 0;
        bufPtr = nullptr;
    }

    bool NextSector() {
        for (;;) {
            // Get byte count from the PRD
            uint16_t byteCount = m_currPRD->byteCount;
            if (byteCount == 0) {
                byteCount = KiB(64);
            }

            // Go to the next PRD if we reached the end of the current PRD
            // and there are more entries
            if (m_currByte >= m_currPRD->byteCount) {
                // No more entries
                if (m_currPRD->endOfTable) {
                    bufPtr = nullptr;
                    return false;
                }

                m_currPRD++;
                continue;
            }

            // Prepare the pointer to the next block
            physAddr = m_currPRD->basePhysicalAddress + m_currByte;
            bufPtr = m_ram + physAddr;
            m_currByte += kSectorSize;
            return true;
        }
    }
};

void BMIDEChannel::RunWorker() {
    while (m_worker_running) {
        // Wait for work
        {
            std::unique_lock<std::mutex> lock(m_jobMutex);
            m_jobCond.wait(lock);
        }

        // Do work
        PRDHelper helper(m_ram, m_ramSize, m_prdTableAddr);

        while (m_job_running) {
            // The manual says that 1 means Bus Master write and 0 means Bus Master read,
            // which is true from the perspective of the bus itself, but confusing to a programmer.
            // From the programmer's perspective, 0 means write to device and 1 means read from device.
            // See https://wiki.osdev.org/ATA/ATAPI_using_DMA#The_Command_Byte
            bool isWrite = (m_command & CmdReadWriteControl) == 0;

            if (m_ataChannel.IsDMAFinished()) {
                // Operation finished; notify ATA channel
                m_job_running = false;

                // Clear Active flag
                m_status &= ~StActive;

                // Set Interrupt flag if the ATA device triggered an interrupt
                if (m_ataChannel.EndDMA()) {
                    m_status |= StInterrupt;
                    m_job_running = false;
                    break;
                }
            }

            bool endDMA = false;
            bool ranOutOfPRDs = false;
            uint8_t buf[kSectorSize];
            if (isWrite) {
                if (helper.NextSector()) {
                    // Copy from physical memory
                    memcpy(buf, helper.bufPtr, kSectorSize);

                    // Do DMA write
                    if (m_ataChannel.WriteDMA(buf)) {
                        // DMA block transfer completed successfully
                        log_spew("BM IDE channel %d: write to physical address 0x%x\n", m_channel, helper.physAddr);
                    }
                    else {
                        // Operation finished; notify ATA channel
                        endDMA = true;
                    }
                }
                else {
                    log_spew("BM IDE channel %d: Ran out of PRDs while writing\n", m_channel);
                    ranOutOfPRDs = true;
                }
            }
            else {
                if (m_ataChannel.ReadDMA(buf)) {
                    // DMA block transfer completed successfully
                    if (helper.NextSector()) {
                        log_spew("BM IDE channel %d: read from physical address 0x%x\n", m_channel, helper.physAddr);

                        // Copy to physical memory
                        memcpy(helper.bufPtr, buf, kSectorSize);
                    }
                    else {
                        log_spew("BM IDE channel %d: Ran out of PRDs while reading\n", m_channel);
                        ranOutOfPRDs = true;
                    }
                }
                else {
                    // Operation finished; notify ATA channel
                    endDMA = true;
                }
            }

            // Handle error conditions
            if (ranOutOfPRDs) {
                // Clear Active flag
                m_status &= ~StActive;

                // Set Interrupt flag if the ATA device triggered an interrupt
                if (m_ataChannel.EndDMA()) {
                    m_status |= StInterrupt;
                }

                m_job_running = false;
            }
            else if (endDMA) {
                if (m_ataChannel.EndDMA()) {
                    // Set Interrupt flag if the ATA device triggered an interrupt
                    m_status |= StInterrupt;
                }

                m_job_running = false;
            }
        }

        if (m_job_cancel) {
            // Clear Active flag if the job was cancelled
            m_status &= ~StActive;
        }
    }
}

}
