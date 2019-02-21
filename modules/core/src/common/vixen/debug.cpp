#include "vixen/debug.h"
//#include <Zydis/Zydis.h>

#include "virt86/virt86.hpp"

#include <cinttypes>
#include <cstdint>

namespace vixen {

using namespace virt86;


#define PRINT_FLAG(flags, prefix, flag) \
do { \
    if (flags & prefix##_##flag) log_debug(" " #flag); \
} while (0)

void printRFLAGSBits(uint64_t rflags) {
    PRINT_FLAG(rflags, RFLAGS, CF);
    PRINT_FLAG(rflags, RFLAGS, PF);
    PRINT_FLAG(rflags, RFLAGS, AF);
    PRINT_FLAG(rflags, RFLAGS, ZF);
    PRINT_FLAG(rflags, RFLAGS, SF);
    PRINT_FLAG(rflags, RFLAGS, TF);
    PRINT_FLAG(rflags, RFLAGS, IF);
    PRINT_FLAG(rflags, RFLAGS, DF);
    PRINT_FLAG(rflags, RFLAGS, OF);
    PRINT_FLAG(rflags, RFLAGS, NT);
    PRINT_FLAG(rflags, RFLAGS, RF);
    PRINT_FLAG(rflags, RFLAGS, VM);
    PRINT_FLAG(rflags, RFLAGS, AC);
    PRINT_FLAG(rflags, RFLAGS, VIF);
    PRINT_FLAG(rflags, RFLAGS, VIP);
    PRINT_FLAG(rflags, RFLAGS, ID);
    uint8_t iopl = (rflags & RFLAGS_IOPL) >> RFLAGS_IOPL_SHIFT;
    log_debug(" IOPL=%u", iopl);
}

void printEFERBits(uint64_t efer) {
    PRINT_FLAG(efer, EFER, SCE);
    PRINT_FLAG(efer, EFER, LME);
    PRINT_FLAG(efer, EFER, LMA);
    PRINT_FLAG(efer, EFER, NXE);
    PRINT_FLAG(efer, EFER, SVME);
    PRINT_FLAG(efer, EFER, LMSLE);
    PRINT_FLAG(efer, EFER, FFXSR);
    PRINT_FLAG(efer, EFER, TCE);
}

void printCR0Bits(uint64_t cr0) {
    PRINT_FLAG(cr0, CR0, PE);
    PRINT_FLAG(cr0, CR0, MP);
    PRINT_FLAG(cr0, CR0, EM);
    PRINT_FLAG(cr0, CR0, TS);
    PRINT_FLAG(cr0, CR0, ET);
    PRINT_FLAG(cr0, CR0, NE);
    PRINT_FLAG(cr0, CR0, WP);
    PRINT_FLAG(cr0, CR0, AM);
    PRINT_FLAG(cr0, CR0, NW);
    PRINT_FLAG(cr0, CR0, CD);
    PRINT_FLAG(cr0, CR0, PG);
}

void printCR4Bits(uint64_t cr4) {
    PRINT_FLAG(cr4, CR4, VME);
    PRINT_FLAG(cr4, CR4, PVI);
    PRINT_FLAG(cr4, CR4, TSD);
    PRINT_FLAG(cr4, CR4, DE);
    PRINT_FLAG(cr4, CR4, PSE);
    PRINT_FLAG(cr4, CR4, PAE);
    PRINT_FLAG(cr4, CR4, MCE);
    PRINT_FLAG(cr4, CR4, PGE);
    PRINT_FLAG(cr4, CR4, PCE);
    PRINT_FLAG(cr4, CR4, OSFXSR);
    PRINT_FLAG(cr4, CR4, OSXMMEXCPT);
    PRINT_FLAG(cr4, CR4, UMIP);
    PRINT_FLAG(cr4, CR4, VMXE);
    PRINT_FLAG(cr4, CR4, SMXE);
    PRINT_FLAG(cr4, CR4, PCID);
    PRINT_FLAG(cr4, CR4, OSXSAVE);
    PRINT_FLAG(cr4, CR4, SMEP);
    PRINT_FLAG(cr4, CR4, SMAP);
}

void printCR8Bits(uint64_t cr8) {
    uint8_t tpr = cr8 & CR8_TPR;
    log_debug(" TPR=%d", tpr);
}

void printXCR0Bits(uint64_t xcr0) {
    PRINT_FLAG(xcr0, XCR0, FP);
    PRINT_FLAG(xcr0, XCR0, SSE);
    PRINT_FLAG(xcr0, XCR0, AVX);
    PRINT_FLAG(xcr0, XCR0, BNDREG);
    PRINT_FLAG(xcr0, XCR0, BNDCSR);
    PRINT_FLAG(xcr0, XCR0, opmask);
    PRINT_FLAG(xcr0, XCR0, ZMM_Hi256);
    PRINT_FLAG(xcr0, XCR0, Hi16_ZMM);
    PRINT_FLAG(xcr0, XCR0, PKRU);
}

void printDR6Bits(uint64_t dr6) {
    PRINT_FLAG(dr6, DR6, BP0);
    PRINT_FLAG(dr6, DR6, BP1);
    PRINT_FLAG(dr6, DR6, BP2);
    PRINT_FLAG(dr6, DR6, BP3);
}

void printDR7Bits(uint64_t dr7) {
    for (uint8_t i = 0; i < 4; i++) {
        if (dr7 & (DR7_LOCAL(i) | DR7_GLOBAL(i))) {
            log_debug(" BP%u[", i);

            if (dr7 & DR7_LOCAL(i)) log_debug("L");
            if (dr7 & DR7_GLOBAL(i)) log_debug("G");

            uint8_t size = (dr7 & DR7_SIZE(i)) >> DR7_SIZE_SHIFT(i);
            switch (size) {
            case DR7_SIZE_BYTE: log_debug(" byte"); break;
            case DR7_SIZE_WORD: log_debug(" word"); break;
            case DR7_SIZE_QWORD: log_debug(" qword"); break;
            case DR7_SIZE_DWORD: log_debug(" dword"); break;
            }

            uint8_t cond = (dr7 & DR7_COND(i)) >> DR7_COND_SHIFT(i);
            switch (cond) {
            case DR7_COND_EXEC: log_debug(" exec"); break;
            case DR7_COND_WIDTH8: log_debug(" width8"); break;
            case DR7_COND_WRITE: log_debug(" write"); break;
            case DR7_COND_READWRITE: log_debug(" r/w"); break;
            }

            log_debug("]");
        }
    }
}
#undef PRINT_FLAG


void DumpCPURegisters(VirtualProcessor& vp) {
#define READREG(code, name) RegValue name; vp.RegRead(code, name);
    READREG(Reg::RAX, rax); READREG(Reg::RCX, rcx); READREG(Reg::RDX, rdx); READREG(Reg::RBX, rbx);
    READREG(Reg::RSP, rsp); READREG(Reg::RBP, rbp); READREG(Reg::RSI, rsi); READREG(Reg::RDI, rdi);
    READREG(Reg::R8, r8); READREG(Reg::R9, r9); READREG(Reg::R10, r10); READREG(Reg::R11, r11);
    READREG(Reg::R12, r12); READREG(Reg::R13, r13); READREG(Reg::R14, r14); READREG(Reg::R15, r15);
    READREG(Reg::RIP, rip);
    READREG(Reg::CS, cs); READREG(Reg::SS, ss);
    READREG(Reg::DS, ds); READREG(Reg::ES, es);
    READREG(Reg::FS, fs); READREG(Reg::GS, gs);
    READREG(Reg::LDTR, ldtr); READREG(Reg::TR, tr);
    READREG(Reg::GDTR, gdtr);
    READREG(Reg::IDTR, idtr);
    READREG(Reg::RFLAGS, rflags);
    READREG(Reg::EFER, efer);
    READREG(Reg::CR2, cr2); READREG(Reg::CR0, cr0);
    READREG(Reg::CR3, cr3); READREG(Reg::CR4, cr4);
    READREG(Reg::DR0, dr0); READREG(Reg::CR8, cr8);
    READREG(Reg::DR1, dr1); READREG(Reg::XCR0, xcr0);
    READREG(Reg::DR2, dr2); READREG(Reg::DR6, dr6);
    READREG(Reg::DR3, dr3); READREG(Reg::DR7, dr7);
#undef READREG

    auto extendedRegs = BitmaskEnum(vp.GetVirtualMachine().GetPlatform().GetFeatures().extendedControlRegisters);

    log_debug(" RAX = %016" PRIx64 "   RCX = %016" PRIx64 "   RDX = %016" PRIx64 "   RBX = %016" PRIx64 "\n", rax.u64, rcx.u64, rdx.u64, rbx.u64);
    log_debug(" RSP = %016" PRIx64 "   RBP = %016" PRIx64 "   RSI = %016" PRIx64 "   RDI = %016" PRIx64 "\n", rsp.u64, rbp.u64, rsi.u64, rdi.u64);
    log_debug("  R8 = %016" PRIx64 "    R9 = %016" PRIx64 "   R10 = %016" PRIx64 "   R11 = %016" PRIx64 "\n", r8.u64, r9.u64, r10.u64, r11.u64);
    log_debug(" R12 = %016" PRIx64 "   R13 = %016" PRIx64 "   R14 = %016" PRIx64 "   R15 = %016" PRIx64 "\n", r12.u64, r13.u64, r14.u64, r15.u64);
    log_debug(" RIP = %016" PRIx64 "\n", rip.u64);
    log_debug("  CS = %04x -> %016" PRIx64 ":%08x [%04x]   SS = %04x -> %016" PRIx64 ":%08x [%04x]\n", cs.segment.selector, cs.segment.base, cs.segment.limit, cs.segment.attributes.u16, ss.segment.selector, ss.segment.base, ss.segment.limit, ss.segment.attributes.u16);
    log_debug("  DS = %04x -> %016" PRIx64 ":%08x [%04x]   ES = %04x -> %016" PRIx64 ":%08x [%04x]\n", ds.segment.selector, ds.segment.base, ds.segment.limit, ds.segment.attributes.u16, es.segment.selector, es.segment.base, es.segment.limit, es.segment.attributes.u16);
    log_debug("  FS = %04x -> %016" PRIx64 ":%08x [%04x]   GS = %04x -> %016" PRIx64 ":%08x [%04x]\n", fs.segment.selector, fs.segment.base, fs.segment.limit, fs.segment.attributes.u16, gs.segment.selector, gs.segment.base, gs.segment.limit, gs.segment.attributes.u16);
    log_debug("LDTR = %04x -> %016" PRIx64 ":%08x [%04x]   TR = %04x -> %016" PRIx64 ":%08x [%04x]\n", ldtr.segment.selector, ldtr.segment.base, ldtr.segment.limit, ldtr.segment.attributes.u16, tr.segment.selector, tr.segment.base, tr.segment.limit, tr.segment.attributes.u16);
    log_debug("GDTR =         %016" PRIx64 ":%04x\n", gdtr.table.base, gdtr.table.limit);
    log_debug("IDTR =         %016" PRIx64 ":%04x\n", idtr.table.base, idtr.table.limit);
    log_debug("RFLAGS = %016" PRIx64 "", rflags.u64); printRFLAGSBits(rflags.u64); log_debug("\n");
    log_debug("EFER = %016" PRIx64, efer.u64); printEFERBits(efer.u64); log_debug("\n");

    log_debug(" CR2 = %016" PRIx64 "   CR0 = %016" PRIx64 "", cr2.u64, cr0.u64); printCR0Bits(cr0.u64); log_debug("\n");
    log_debug(" CR3 = %016" PRIx64 "   CR4 = %016" PRIx64 "", cr3.u64, cr4.u64); printCR4Bits(cr4.u64); log_debug("\n");
    log_debug(" DR0 = %016" PRIx64 "   CR8 = ", dr0.u64);
    if (extendedRegs.AnyOf(ExtendedControlRegister::CR8)) {
        log_debug("%016" PRIx64 "", cr8.u64); printCR8Bits(cr8.u64); log_debug("\n");
    }
    else {
        log_debug("................\n");
    }
    log_debug(" DR1 = %016" PRIx64 "  XCR0 = ", dr1.u64);
    if (extendedRegs.AnyOf(ExtendedControlRegister::XCR0)) {
        log_debug("%016" PRIx64 "", xcr0.u64); printXCR0Bits(xcr0.u64); log_debug("\n");
    }
    else {
        log_debug("................\n");
    }
    log_debug(" DR2 = %016" PRIx64 "   DR6 = %016" PRIx64 "", dr2.u64, dr6.u64); printDR6Bits(dr6.u64); log_debug("\n");
    log_debug(" DR3 = %016" PRIx64 "   DR7 = %016" PRIx64 "", dr3.u64, dr7.u64); printDR7Bits(dr7.u64); log_debug("\n");
}

void DumpCPUStack(VirtualProcessor& vp, int32_t offsetStart, int32_t offsetEnd) {
    RegValue esp, ebp;
    vp.RegRead(Reg::ESP, esp);
    vp.RegRead(Reg::EBP, ebp);
    log_debug("Stack:\n");
    for (int i = offsetStart; i <= offsetEnd; i += 0x4) {
        uint32_t val;
        if (vp.LMemRead(esp.u32 + i, 4, &val)) {
            log_debug("%s %08x  [esp%c%02x]  =  %08x%s\n", ((i == 0) ? "=>" : "  "), esp.u32 + i, ((i < 0) ? '-' : '+'), ((i < 0) ? -i : i), val, ((esp.u32 + i) == ebp.u32) ? " <= EBP" : "");
        }
        else {
            log_debug("%s %08x  [esp%c%02x]  =  ........%s\n", ((i == 0) ? "=>" : "  "), esp.u32 + i, ((i < 0) ? '-' : '+'), ((i < 0) ? -i : i), ((esp.u32 + i) == ebp.u32) ? " <= EBP" : "");
        }
    }
    log_debug("\n");
}

void DumpCPUMemory(VirtualProcessor& vp, uint32_t address, uint32_t size, bool physical) {
	log_debug("%s memory at 0x%08x:\n", (physical ? "Physical" : "Linear"), address);
	char *mem = new char[size];
	if (physical) {
		if (vp.MemRead(address, size, mem)) {
			log_debug("<invalid address>\n\n");
			delete[] mem;
			return;
		}
	}
	else {
		if (vp.LMemRead(address, size, mem)) {
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

void DumpCPUDisassembly(VirtualProcessor& vp, uint32_t address, uint32_t count, bool physical) {
    // TODO: fix CMakeLists.txt to include Zydis
	/*log_debug("%s memory disassembly at 0x%08x:\n", (physical ? "Physical" : "Virtual"), address);
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
	}*/
}

}
