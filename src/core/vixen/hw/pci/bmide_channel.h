// Bus Master PCI IDE Controller emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// Based on the Programming Interface for Bus Master IDE Controller
// Revision 1.0 available at http://www.bswd.com/idems100.pdf
// and complemented with information from PC87415 at
// https://parisc.wiki.kernel.org/images-parisc/0/0a/PC87415.pdf
#pragma once

#include <cstdint>
#include <mutex>

#include "bmide/defs.h"

namespace vixen {

class BMIDEChannel {
public:
    BMIDEChannel(hw::bmide::Channel channel);
    ~BMIDEChannel();
private:
    friend class BMIDEDevice;
    
    hw::bmide::Channel m_channel;

    // ----- Registers --------------------------------------------------------

    uint8_t m_command = 0;
    uint8_t m_status = 0;
    uint32_t m_prdTableAddrs = 0;

    // ----- Operations -------------------------------------------------------

    void ReadCommand(uint32_t *value, uint8_t size);
    void ReadStatus(uint32_t *value, uint8_t size);
    void ReadPRDTableAddress(uint32_t *value, uint8_t size);

    void WriteCommand(uint32_t value, uint8_t size);
    void WriteStatus(uint32_t value, uint8_t size);
    void WritePRDTableAddress(uint32_t value, uint8_t size);

    // ----- Worker thread ----------------------------------------------------

    std::thread m_workerThread;
    static uint32_t WorkerThreadFunc(void *data);
    void RunWorker();

    std::mutex m_jobMutex;
    std::condition_variable m_jobCond;
    bool m_worker_running;
    bool m_job_running;

    void BeginWork();
    void StopWork();
};

}
