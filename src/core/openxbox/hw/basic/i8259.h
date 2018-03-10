#pragma once

#include <cstdint>

#include "openxbox/cpu.h"

namespace openxbox {

#define PORT_PIC_MASTER_COMMAND  0x20
#define PORT_PIC_MASTER_DATA     0x21
#define PORT_PIC_SLAVE_COMMAND   0xA0
#define PORT_PIC_SLAVE_DATA      0xA1
#define PORT_PIC_MASTER_ELCR     0x4D0
#define PORT_PIC_SLAVE_ELCR      0x4D1

#define PIC_MASTER	0
#define PIC_SLAVE	1

class i8259 {
public:
	i8259(Cpu *cpu);
	void Reset();

	void IORead(uint32_t addr, uint32_t *value, uint16_t size);
	void IOWrite(uint32_t addr, uint32_t value, uint16_t size);

	void RaiseIRQ(int index);
	void LowerIRQ(int index);

	int GetCurrentIRQ();
private:
	Cpu *m_cpu;

	uint8_t m_PreviousIRR[2];	// used for edge-detection
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
};

}
