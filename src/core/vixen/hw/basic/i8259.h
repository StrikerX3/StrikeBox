#pragma once

#include <cstdint>

#include "vixen/cpu.h"
#include "irq.h"

#include <mutex>

namespace vixen {

using namespace vixen::cpu;

#define PORT_PIC_MASTER_COMMAND  0x20
#define PORT_PIC_MASTER_DATA     0x21
#define PORT_PIC_SLAVE_COMMAND   0xA0
#define PORT_PIC_SLAVE_DATA      0xA1
#define PORT_PIC_MASTER_ELCR     0x4D0
#define PORT_PIC_SLAVE_ELCR      0x4D1

#define PORT_PIC_MASTER_BASE     PORT_PIC_MASTER_COMMAND
#define PORT_PIC_SLAVE_BASE      PORT_PIC_SLAVE_COMMAND
#define PORT_PIC_ELCR_BASE       PORT_PIC_MASTER_ELCR
#define PORT_PIC_COUNT           2

#define PIC_MASTER    0
#define PIC_SLAVE    1

class i8259 : public IODevice, public IRQHandler {
public:
    i8259(Cpu& cpu);
    virtual ~i8259();
    void Reset();

    bool MapIO(IOMapper *mapper);

    bool IORead(uint32_t port, uint32_t *value, uint8_t size) override;
    bool IOWrite(uint32_t port, uint32_t value, uint8_t size) override;

    void HandleIRQ(uint8_t irqNum, bool level) override;

    int GetCurrentIRQ();
private:
    Cpu&    m_cpu;

    uint8_t m_PreviousIRR[2];    // used for edge-detection
    uint8_t m_IRR[2];
    uint8_t m_IMR[2];
    uint8_t m_ISR[2];
    uint8_t m_Base[2];
    uint8_t m_ReadRegisterSelect[2];
    uint8_t m_SpecialMask[2];
    uint8_t m_InitState[2];
    uint8_t m_ELCR[2];
    uint8_t m_ELCRMask[2];
    uint8_t m_PriorityAdd[2];

    bool m_Poll[2];
    bool m_RotateOnAutoEOI[2];
    bool m_Is4ByteInit[2];
    bool m_InterruptOutput[2];
    bool m_AutoEOI[2];
    bool m_IsSpecialFullyNestedMode[2];

    std::mutex m_handleIRQMutex;

    uint32_t CommandRead(int pic);
    void CommandWrite(int pic, uint32_t value);
    uint32_t DataRead(int pic);
    void DataWrite(int pic, uint32_t value);

    void AcknowledgeIRQ(int pic, int index);
    int GetIRQ(int pic);
    void SetIRQ(int pic, int index, bool value);
    int GetPriority(int pic, uint8_t mask);
    uint8_t Poll(int pic);
    void Reset(int pic);
    void UpdateIRQ(int pic);

    void RaiseIRQ(int index);
    void LowerIRQ(int index);
};

}
