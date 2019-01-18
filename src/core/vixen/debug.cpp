#include "debug.h"
#include <Zydis/Zydis.h>

namespace vixen {

using namespace vixen::cpu;

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

    CpuReg regs[] = {
        REG_CS, REG_EIP, REG_EBP,
        REG_SS, REG_EAX, REG_ESP,
        REG_DS, REG_ECX, REG_CR0,
        REG_ES, REG_EDX, REG_CR2,
        REG_FS, REG_EBX, REG_CR3,
        REG_GS, REG_ESI, REG_CR4,
        REG_TR, REG_EDI, REG_EFLAGS,
    };
    uint32_t vals[ARRAY_SIZE(regs)];

    cpu->RegRead(regs, vals, ARRAY_SIZE(regs));
	
	/*                          */ log_debug("CPU registers:\n");
    /*                          */ log_debug(" CS = %04x  EIP = %08x  EBP = %08x\n", vals[0], vals[1], vals[2]);
    /*                          */ log_debug(" SS = %04x  EAX = %08x  ESP = %08x\n", vals[3], vals[4], vals[5]);
    parseCR0Flags(vals[8], temp);  log_debug(" DS = %04x  ECX = %08x  CR0 = %08x  %s\n", vals[6], vals[7], vals[8], temp);
    /*                          */ log_debug(" ES = %04x  EDX = %08x  CR2 = %08x\n", vals[9], vals[10], vals[11]);
    /*                          */ log_debug(" FS = %04x  EBX = %08x  CR3 = %08x\n", vals[12], vals[13], vals[14]);
    parseCR4Flags(vals[17], temp); log_debug(" GS = %04x  ESI = %08x  CR4 = %08x  %s\n", vals[15], vals[16], vals[17], temp);
    parseEFlags(vals[20], temp);   log_debug(" TR = %04x  EDI = %08x  EFL = %08x  %s\n", vals[18], vals[19], vals[20], temp);


	uint32_t base;
	uint32_t limit;
	cpu->GetGDT(&base, &limit); log_debug("GDT = %08x:%04x\n", base, limit);
	cpu->GetIDT(&base, &limit); log_debug("IDT = %08x:%04x\n", base, limit);
	log_debug("\n");
}

void DumpCPUStack(Cpu *cpu, int32_t offsetStart, int32_t offsetEnd) {
    uint32_t esp;
    uint32_t ebp;
    cpu->RegRead(REG_ESP, &esp);
    cpu->RegRead(REG_EBP, &ebp);
    log_debug("Stack:\n");
    for (int i = offsetStart; i <= offsetEnd; i += 0x4) {
        uint32_t val;
        CPUOperationStatus result = cpu->VMemRead(esp + i, 4, &val);
        if (result == CPUS_OP_OK) {
            log_debug("%s %08x  [esp%c%02x]  =  %08x%s\n", ((i == 0) ? "=>" : "  "), esp + i, ((i < 0) ? '-' : '+'), ((i < 0) ? -i : i), val, ((esp + i) == ebp) ? " <= EBP" : "");
        }
        else {
            log_debug("%s %08x  [esp%c%02x]  =  ........%s\n", ((i == 0) ? "=>" : "  "), esp + i, ((i < 0) ? '-' : '+'), ((i < 0) ? -i : i), ((esp + i) == ebp) ? " <= EBP" : "");
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

void DumpCPUDisassembly(Cpu* cpu, uint32_t address, uint32_t count, bool physical) {
	log_debug("%s memory disassembly at 0x%08x:\n", (physical ? "Physical" : "Virtual"), address);
    uint8_t mem[16];

	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder,
		ZYDIS_MACHINE_MODE_LEGACY_32,
		ZYDIS_ADDRESS_WIDTH_32);

	ZydisFormatter formatter;
	ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

	ZydisDecodedInstruction instruction;

	for (uint32_t i = 0; i < count; i++) {
        if (physical) {
            if (cpu->MemRead(address, 16, mem)) {
                log_debug("<invalid address>\n\n");
                return;
            }
        }
        else {
            if (cpu->VMemRead(address, 16, mem)) {
                log_debug("<invalid address>\n\n");
                return;
            }
        }

        auto result = ZydisDecoderDecodeBuffer(&decoder, mem, 16, address, &instruction);
		if (ZYDIS_SUCCESS(result)) {
			char buffer[256];
			ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer));
			log_debug("%08x  %s\n", address, buffer);
		}
		address += instruction.length;
	}
}

}
