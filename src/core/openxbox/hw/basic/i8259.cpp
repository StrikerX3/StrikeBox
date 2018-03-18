#include "i8259.h"

#include "openxbox/log.h"
#include "openxbox/io.h"

namespace openxbox {

// Implementation based on QEMU:
// https://github.com/qemu/qemu/blob/master/hw/intc/i8259_common.c
// https://github.com/qemu/qemu/blob/master/hw/intc/i8259.c

// With additional modifications from XQEMU:
// https://github.com/xqemu/xqemu/commit/ad6aef5a665bec48153f300ca7ccd41423d5d7ff#diff-6dd7dc24a8a5780de84f792316cc450d
// https://github.com/xqemu/xqemu/commit/32cba22156197c7371cca4f54952c6f7dfe6ed9c#diff-565ee39a6444946e5a0055218276766d


#define ICW1 0
#define ICW2 1
#define ICW3 2
#define ICW4 3

#define ICW1_ICW4       0x01    /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08    /* Level triggered (edge) mode */
#define ICW1_INIT       0x10    /* Initialization - required! */

#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM       0x10    /* Special fully nested (not) */

#define PIC_READ_IRR    0       /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR    1       /* OCW3 irq service next CMD read */
#define PIC_EOI         0x20

// TODO: Implement ELCR support

i8259::i8259(Cpu *cpu) {
    m_cpu = cpu;
}

i8259::~i8259() {
}

void i8259::Reset() {
    Reset(PIC_MASTER);
    Reset(PIC_SLAVE);

    m_ELCR[PIC_MASTER] = 0;
    m_ELCR[PIC_SLAVE] = 0;

    m_ELCRMask[PIC_MASTER] = 0xF8;
    m_ELCRMask[PIC_SLAVE] = 0xDE;
}

void i8259::Reset(int pic) {
    m_PreviousIRR[pic] = 0;
    m_IRR[pic] = 0;
    m_IMR[pic] = 0;
    m_ISR[pic] = 0;
    m_PriorityAdd[pic] = 0;
    m_Base[pic] = 0;
    m_ReadRegisterSelect[pic] = 0;
    m_Poll[pic] = false;
    m_SpecialMask[pic] = 0;
    m_InitState[pic] = 0;
    m_AutoEOI[pic] = false;
    m_RotateOnAutoEOI[pic] = false;
    m_IsSpecialFullyNestedMode[pic] = false;
    m_Is4ByteInit[pic] = false;
    m_InterruptOutput[pic] = false;

    UpdateIRQ(pic);
}

bool i8259::MapIO(IOMapper *mapper) {
    if (!mapper->MapIODevice(PORT_PIC_MASTER_BASE, PORT_PIC_COUNT, this)) return false;
    if (!mapper->MapIODevice(PORT_PIC_SLAVE_BASE, PORT_PIC_COUNT, this)) return false;
    if (!mapper->MapIODevice(PORT_PIC_ELCR_BASE, PORT_PIC_COUNT, this)) return false;

    return true;
}

void i8259::SetIRQ(int pic, int index, bool asserted) {
    int mask = 1 << index;

    // Level Triggered
    if (m_ELCR[pic] & mask) {
        if (asserted) {
            m_IRR[pic] |= mask;
            m_PreviousIRR[pic] |= mask;
        }
        else {
            m_IRR[pic] &= ~mask;
            m_PreviousIRR[pic] &= ~mask;
        }

        UpdateIRQ(pic);
        return;
    }

    // Edge Triggered
    if (asserted) {
        if ((m_PreviousIRR[pic] & mask) == 0) {
            m_IRR[pic] |= mask;
        }
        m_PreviousIRR[pic] |= mask;
    }
    else {
        m_IRR[pic] &= ~mask;
        m_PreviousIRR[pic] &= ~mask;
    }

    UpdateIRQ(pic);
}

bool i8259::IORead(uint32_t port, uint32_t *value, uint8_t size) {
    switch (port) {
    case PORT_PIC_MASTER_COMMAND:
        *value = CommandRead(PIC_MASTER);
        return true;
    case PORT_PIC_SLAVE_COMMAND:
        *value = CommandRead(PIC_SLAVE);
        return true;
    case PORT_PIC_MASTER_DATA:
        *value = DataRead(PIC_MASTER);
        return true;
    case PORT_PIC_SLAVE_DATA:
        *value = DataRead(PIC_SLAVE);
        return true;
    case PORT_PIC_MASTER_ELCR:
        *value = m_ELCR[PIC_MASTER];
        return true;
    case PORT_PIC_SLAVE_ELCR:
        *value = m_ELCR[PIC_SLAVE];
        return true;
    }

    log_warning("i8259::IORead:  Invalid address 0x%x\n", port);
    return false;
}

bool i8259::IOWrite(uint32_t port, uint32_t value, uint8_t size) {
    switch (port) {
    case PORT_PIC_MASTER_COMMAND:
        CommandWrite(PIC_MASTER, value);
        return true;
    case PORT_PIC_SLAVE_COMMAND:
        CommandWrite(PIC_SLAVE, value);
        return true;
    case PORT_PIC_MASTER_DATA:
        DataWrite(PIC_MASTER, value);
        return true;
    case PORT_PIC_SLAVE_DATA:
        DataWrite(PIC_SLAVE, value);
        return true;
    case PORT_PIC_MASTER_ELCR:
        m_ELCR[PIC_MASTER] = value & m_ELCRMask[PIC_MASTER];
        return true;
    case PORT_PIC_SLAVE_ELCR:
        m_ELCR[PIC_SLAVE] = value & m_ELCRMask[PIC_SLAVE];
        return true;
    }

    log_warning("i8259::IOWrite: Invalid address 0x%x\n", port);
    return false;
}

void i8259::HandleIRQ(uint8_t irqNum, int level) {
    std::lock_guard<std::mutex> lk(m_handleIRQMutex);

    if (level) {
        RaiseIRQ(irqNum);
    }
    else {
        LowerIRQ(irqNum);
    }
}

uint32_t i8259::CommandRead(int pic) {
    if (m_Poll[pic]) {
        return Poll(pic);
    }

    if (m_ReadRegisterSelect) {
        return m_ISR[pic];
    }

    return m_IRR[pic];
}

void i8259::CommandWrite(int pic, uint32_t value) {
    if (value & 0x10) {
        Reset(pic);
        m_InitState[pic] = 1;
        m_Is4ByteInit[pic] = value & 1;
        if (value & 0x08) {
            log_warning("i8259::CommandWrite: Level sensitive IRQ not supported\n");
        }

        return;
    }

    if (value & 0x08) {
        if (value & 0x04) {
            m_Poll[pic] = true;
        }

        if (value & 0x02) {
            m_ReadRegisterSelect[pic] = value & 1;
        }

        if (value & 0x40) {
            m_SpecialMask[pic] = (value >> 5) & 1;
        }

        return;
    }

    int command = value >> 5;

    switch (command) {
    case 0:
    case 4:
        m_RotateOnAutoEOI[pic] = command >> 2;
        break;
    case 1:
    case 5:
    {
        int priority = GetPriority(pic, m_ISR[pic]);
        if (priority == 8) {
            return;
        }

        int irq = (priority + m_PriorityAdd[pic]) & 7;
        m_ISR[pic] &= ~(1 << irq);

        if (command == 5) {
            m_PriorityAdd[pic] = (irq + 1) & 7;
        }

        UpdateIRQ(pic);
        break;
    }
    case 3:
    {
        int irq = value & 7;
        m_ISR[pic] &= ~(1 << irq);
        UpdateIRQ(pic);
        break;
    }
    case 6:
        m_PriorityAdd[pic] = (value + 1) & 7;
        UpdateIRQ(pic);
        break;
    case 7:
        int irq = value & 7;
        m_ISR[pic] &= ~(1 << irq);
        m_PriorityAdd[pic] = (irq + 1) & 7;
        UpdateIRQ(pic);
        break;
    }
}

uint32_t i8259::DataRead(int pic) {
    if (m_Poll[pic]) {
        return Poll(pic);
    }

    return m_IMR[pic];
}

void i8259::DataWrite(int pic, uint32_t value) {
    switch (m_InitState[pic]) {
    case 0:
        m_IMR[pic] = value;
        UpdateIRQ(pic);
        break;
    case 1:
        m_Base[pic] = value & 0xF8;
        m_InitState[pic] = 2;
        break;
    case 2:
        if (m_Is4ByteInit[pic]) {
            m_InitState[pic] = 3;
        }
        else {
            m_InitState[pic] = 0;
        }
        break;
    case 3:
        m_IsSpecialFullyNestedMode[pic] = (value >> 4) & 1;
        m_AutoEOI[pic] = (value >> 1) & 1;
        m_InitState[pic] = 0;
        break;
    }
}

int i8259::GetCurrentIRQ() {
    int masterIrq = GetIRQ(PIC_MASTER);

    // If this was a spurious IRQ, report it as such
    if (masterIrq < 0) {
        return m_Base[PIC_MASTER] + 7;
    }

    // If the master IRQ didn't come from the slave
    if (masterIrq != 2) {
        AcknowledgeIRQ(PIC_MASTER, masterIrq);
        return m_Base[PIC_MASTER] + masterIrq;
    }

    int slaveIrq = GetIRQ(PIC_SLAVE);

    // If slaveIrq was a spurious IRQ, report it as such
    if (slaveIrq < 0) {
        return m_Base[PIC_SLAVE] + 7;
    }

    AcknowledgeIRQ(PIC_SLAVE, slaveIrq);
    return m_Base[PIC_SLAVE] + slaveIrq;
}

int i8259::GetPriority(int pic, uint8_t mask) {
    if (mask == 0) {
        return 8;
    }

    int priority = 0;
    while ((mask & (1 << ((priority + m_PriorityAdd[pic]) & 7))) == 0) {
        priority++;
    }

    return priority;
}

int i8259::GetIRQ(int pic) {
    int mask = m_IRR[pic] & ~m_IMR[pic];
    int priority = GetPriority(pic, mask);
    if (priority == 8) {
        return -1;
    }

    mask = m_ISR[pic];

    if (m_SpecialMask[pic]) {
        mask &= ~m_IMR[pic];
    }

    if (m_IsSpecialFullyNestedMode[pic] && pic == PIC_MASTER) {
        mask &= ~(1 << 2);
    }

    int currentPriority = GetPriority(pic, mask);
    if (priority < currentPriority) {
        return (priority + m_PriorityAdd[pic]) & 7;
    }

    return -1;
}

void i8259::AcknowledgeIRQ(int pic, int index) {
    if (m_AutoEOI[pic]) {
        if (m_RotateOnAutoEOI[pic]) {
            m_PriorityAdd[pic] = (index + 1) & 7;
        }
    }
    else {
        m_ISR[pic] |= (1 << index);
    }

    if (!(m_ELCR[pic] & (1 << index))) {
        m_IRR[pic] &= ~(1 << index);
    }

    UpdateIRQ(pic);
}

uint8_t i8259::Poll(int pic) {
    m_Poll[pic] = false;

    int irq = GetIRQ(pic);
    if (irq >= 0) {
        AcknowledgeIRQ(pic, irq);
        return irq | 0x80;
    }

    return 0;
}

void i8259::UpdateIRQ(int pic) {
    int irq = GetIRQ(pic);

    if (irq >= 0) {
        m_InterruptOutput[pic] = true;
    }
    else {
        m_InterruptOutput[pic] = false;
    }

    // If this was the slave PIC, cascade to master
    if (pic == PIC_SLAVE) {
        SetIRQ(PIC_MASTER, 2, m_InterruptOutput[pic]);
    }

    // If this was the master PIC, trigger the IRQ
    if (pic == PIC_MASTER && m_InterruptOutput[PIC_MASTER]) {
        m_cpu->Interrupt(GetCurrentIRQ());
    }
}

void i8259::RaiseIRQ(int index) {
    if (index <= 7) {
        SetIRQ(PIC_MASTER, index, true);
    }
    else {
        SetIRQ(PIC_SLAVE, index - 7, true);
    }
}

void i8259::LowerIRQ(int index) {
    if (index <= 7) {
        SetIRQ(PIC_MASTER, index, false);
    }
    else {
        SetIRQ(PIC_SLAVE, index - 7, false);
    }
}

}
