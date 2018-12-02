#include "debug.h"
#include <Zydis/Zydis.h>

namespace openxbox {

static void parseEFlags(uint32_t flags, char *str) {
	str[0] = 0;
	if (flags & CF_MASK ) { strcat(str, "CF "); }
	if (flags & PF_MASK ) { strcat(str, "PF "); }
	if (flags & AF_MASK ) { strcat(str, "AF "); }
	if (flags & ZF_MASK ) { strcat(str, "ZF "); }
	if (flags & SF_MASK ) { strcat(str, "SF "); }
	if (flags & TF_MASK ) { strcat(str, "TF "); }
	if (flags & IF_MASK ) { strcat(str, "IF "); }
	if (flags & DF_MASK ) { strcat(str, "DF "); }
	if (flags & OF_MASK ) { strcat(str, "OF "); }
	if (flags & NT_MASK ) { strcat(str, "NT "); }
	if (flags & RF_MASK ) { strcat(str, "RF "); }
	if (flags & VM_MASK ) { strcat(str, "VM "); }
	if (flags & AC_MASK ) { strcat(str, "AC "); }
	if (flags & VIF_MASK) { strcat(str, "VIF "); }
	if (flags & VIP_MASK) { strcat(str, "VIP "); }
	strcat(str, "IOPL#");
	auto index = strlen(str) - 1;
	str[index] = ((flags & IOPL_MASK) >> IOPL_BIT0) + '0';
}

static void parseCR0Flags(uint32_t flags, char *str) {
	str[0] = 0;
	if (flags & CR0_PG) { strcat(str, "PG "); }
	if (flags & CR0_CD) { strcat(str, "CD "); }
	if (flags & CR0_NW) { strcat(str, "NW "); }
	if (flags & CR0_AM) { strcat(str, "AM "); }
	if (flags & CR0_WP) { strcat(str, "WP "); }
	if (flags & CR0_NE) { strcat(str, "NE "); }
	if (flags & CR0_ET) { strcat(str, "ET "); }
	if (flags & CR0_TS) { strcat(str, "TS "); }
	if (flags & CR0_EM) { strcat(str, "EM "); }
	if (flags & CR0_MP) { strcat(str, "MP "); }
	if (flags & CR0_PE) { strcat(str, "PE "); }
	if (flags) { str[strlen(str) - 1] = 0; }
}

static void parseCR4Flags(uint32_t flags, char *str) {
	str[0] = 0;
	if (flags & CR4_VME) { strcat(str, "VME "); }
	if (flags & CR4_PVI) { strcat(str, "PVI "); }
	if (flags & CR4_TSD) { strcat(str, "TSD "); }
	if (flags & CR4_DE ) { strcat(str, "DE "); }
	if (flags & CR4_PSE) { strcat(str, "PSE "); }
	if (flags & CR4_PAE) { strcat(str, "PAE "); }
	if (flags & CR4_MCE) { strcat(str, "MCE "); }
	if (flags & CR4_PGE) { strcat(str, "PGE "); }
	if (flags & CR4_FXSR) { strcat(str, "FXSR "); }
	if (flags & CR4_XMMEXCPT) { strcat(str, "XMMEXCPT "); }
	if (flags) { str[strlen(str) - 1] = 0; }
}


void DumpCPURegisters(Cpu *cpu) {
	char temp[128];
	uint32_t value;
	
	log_debug("CPU registers:\n");
	cpu->RegRead(REG_CS, &value); log_debug(" CS = %04x  ", value);
	cpu->RegRead(REG_EIP, &value); log_debug("EIP = %08x  ", value);
	cpu->RegRead(REG_EBP, &value); log_debug("EBP = %08x\n", value);

	cpu->RegRead(REG_SS, &value); log_debug(" SS = %04x  ", value);
	cpu->RegRead(REG_EAX, &value); log_debug("EAX = %08x  ", value);
	cpu->RegRead(REG_ESP, &value); log_debug("ESP = %08x\n", value);

	cpu->RegRead(REG_DS, &value); log_debug(" DS = %04x  ", value);
	cpu->RegRead(REG_ECX, &value); log_debug("ECX = %08x  ", value);
	cpu->RegRead(REG_CR0, &value); parseCR0Flags(value, temp); log_debug("CR0 = %08x  %s\n", value, temp);

	cpu->RegRead(REG_ES, &value); log_debug(" ES = %04x  ", value);
	cpu->RegRead(REG_EDX, &value); log_debug("EDX = %08x  ", value);
	cpu->RegRead(REG_CR2, &value); log_debug("CR2 = %08x\n", value);

	cpu->RegRead(REG_FS, &value); log_debug(" FS = %04x  ", value);
	cpu->RegRead(REG_EBX, &value); log_debug("EBX = %08x  ", value);
	cpu->RegRead(REG_CR3, &value); log_debug("CR3 = %08x\n", value);

	cpu->RegRead(REG_GS, &value); log_debug(" GS = %04x  ", value);
	cpu->RegRead(REG_ESI, &value); log_debug("ESI = %08x  ", value);
	cpu->RegRead(REG_CR4, &value); parseCR4Flags(value, temp); log_debug("CR4 = %08x  %s\n", value, temp);

	cpu->RegRead(REG_TR, &value); log_debug(" TR = %04x  ", value);
	cpu->RegRead(REG_EDI, &value); log_debug("EDI = %08x  ", value);
	cpu->RegRead(REG_EFLAGS, &value); parseEFlags(value, temp); log_debug("EFL = %08x  %s\n", value, temp);


	uint32_t base;
	uint32_t limit;
	cpu->GetGDT(&base, &limit); log_debug("GDT = %08x:%04x\n", base, limit);
	cpu->GetIDT(&base, &limit); log_debug("IDT = %08x:%04x\n", base, limit);
	log_debug("\n");
}

void DumpCPUStack(Cpu *cpu, int32_t offsetStart, int32_t offsetEnd) {
	uint32_t esp;
	cpu->RegRead(REG_ESP, &esp);
	log_debug("Stack:\n");
	for (int i = offsetStart; i <= offsetEnd; i += 0x4) {
		uint32_t val;
		int result = cpu->VMemRead(esp + i, 4, &val);
        if (result == 0) {
            log_debug("%s %08x  [esp%c%02x]  =  %08x\n", ((i == 0) ? "=>" : "  "), esp + i, ((i < 0) ? '-' : '+'), ((i < 0) ? -i : i), val);
        }
        else {
            log_debug("%s %08x  [esp%c%02x]  =  ........\n", ((i == 0) ? "=>" : "  "), esp + i, ((i < 0) ? '-' : '+'), ((i < 0) ? -i : i));
        }
	}
	log_debug("\n");
}

void DumpCPUMemory(Cpu *cpu, uint32_t address, uint32_t size, bool physical) {
	log_debug("%s memory at 0x%08x:\n", (physical ? "Physical" : "Virtual"), address);
	char *mem = new char[size];
	if (physical) {
		if (cpu->MemRead(address, size, mem)) {
			log_debug("<invalid address>\n\n");
			delete[] mem;
			return;
		}
	}
	else {
		if (cpu->VMemRead(address, size, mem)) {
			log_debug("<invalid address>\n\n");
			delete[] mem;
			return;
		}
	}

	for (uint32_t i = 0x00; i < size; i++) {
		if ((i & 0xF) == 0x0) {
			log_debug("%08x  ", address + i);
		}
		if ((i & 0xF) == 0x8) {
			log_debug(" ");
		}
		log_debug("%02x ", mem[i] & 0xFF);
		if ((i & 0xF) == 0xF) {
			log_debug("\n");
		}
	}
	if (size & 0xF) { log_debug("\n"); }
	log_debug("\n");

	delete[] mem;
}

void DisassembleCPUMemory(Cpu* cpu, uint32_t address, uint32_t size, bool physical) {
	log_debug("%s memory disassembly at 0x%08x:\n", (physical ? "Physical" : "Virtual"), address);
	char *mem = new char[size];
	if (physical) {
		if (cpu->MemRead(address, size, mem)) {
			log_debug("<invalid address>\n\n");
			delete[] mem;
			return;
		}
	}
	else {
		if (cpu->VMemRead(address, size, mem)) {
			log_debug("<invalid address>\n\n");
			delete[] mem;
			return;
		}
	}

	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder,
		ZYDIS_MACHINE_MODE_LEGACY_32,
		ZYDIS_ADDRESS_WIDTH_32);

	ZydisFormatter formatter;
	ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

	uint32_t offset = 0;
	ZydisDecodedInstruction instruction;

	while (offset < size) {
		if (ZYDIS_SUCCESS(ZydisDecoderDecodeBuffer(
			&decoder, (char*)mem + offset, ((sizeof(char) * size) - offset),
			(address + offset), &instruction
		))) {
			char buffer[256];
			ZydisFormatterFormatInstruction(
				&formatter, &instruction, buffer, sizeof(buffer)
			);
			log_debug("%08x  %s\n", (address + offset), buffer);
		}
		offset += instruction.length;
	}

	delete[] mem;
}

}
