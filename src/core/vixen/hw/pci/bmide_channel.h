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

#include "bmide/bmide_defs.h"
#include "vixen/hw/ata/ata_common.h"
#include "vixen/hw/ata/ata.h"

namespace vixen {
namespace hw {
namespace bmide {

class BMIDEChannel {
public:
    BMIDEChannel(hw::ata::Channel channel, hw::ata::ATAChannel& ataChannel, uint8_t *ram, uint32_t ramSize);
    ~BMIDEChannel();
private:
    friend class BMIDEDevice;
    
    hw::ata::Channel m_channel;
    hw::ata::ATAChannel& m_ataChannel;

    // ----- System memory ----------------------------------------------------

    uint8_t *m_ram = nullptr;
    uint32_t m_ramSize = 0;

    // ----- Registers --------------------------------------------------------

    uint8_t m_command = 0;
    uint8_t m_status = 0;
    uint32_t m_prdTableAddr = 0;

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
    bool m_job_cancel;

    void StartWork();
    void StopWork();

    // ----- Interrupt hook ---------------------------------------------------

    class IntrHook : public hw::InterruptHook {
    private:
        IntrHook(BMIDEChannel& channel)
            : m_channel(channel)
        {
        }

        void OnChange(bool asserted) override {
            if (asserted) {
                m_channel.m_status |= StInterrupt;
                log_spew("BM IDE channel %d:  Interrupt asserted\n", m_channel.m_channel);
            }
        }
        
        BMIDEChannel& m_channel;

        friend class BMIDEChannel;
    };
    friend class IntrHook;

    IntrHook m_intrHook;
};

}
}
}
