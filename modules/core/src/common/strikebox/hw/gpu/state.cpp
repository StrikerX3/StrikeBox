#include "strikebox/hw/gpu/state.h"

#include "strikebox/log.h"

namespace strikebox::nv2a {

NV2A::NV2A(uint8_t* systemRAM, uint32_t systemRAMSize, PCIConfigReader pciCfgReader, PCIConfigWriter pciCfgWriter)
    : systemRAM(systemRAM)
    , systemRAMSize(systemRAMSize)
    , pciCfgReader(pciCfgReader)
    , pciCfgWriter(pciCfgWriter)
{
    RegisterEngine(*pmc);
    RegisterEngine(*pbus);
    RegisterEngine(*pfifo);
    RegisterEngine(*prma);
    RegisterEngine(*pvideo);
    RegisterEngine(*ptimer);
    RegisterEngine(*pcounter);
    RegisterEngine(*pnvio);
    RegisterEngine(*pfb);
    RegisterEngine(*pstraps);
    RegisterEngine(*prom);
    RegisterEngine(*pgraph);
    RegisterEngine(*pcrtc);
    RegisterEngine(*prmcio);
    RegisterEngine(*pramdac);
    RegisterEngine(*prmdio);
    RegisterEngine(*pramin);
    RegisterEngine(*user);
}

void NV2A::Reset() {
    for (auto& eng : engines) {
        eng.second.Reset();
    }
}

uint32_t NV2A::Read(const uint32_t addr, const uint8_t size) {
    auto opt_eng = FindEngine(addr);
    if (opt_eng) {
        auto& eng = opt_eng->get();
        // Aligned 32-bit read as expected
        if ((addr & 3) == 0 && size == 4) {
            return eng.Read(addr - eng.GetOffset());
        }

        // Unaligned or non 32-bit read
        log_warning("NV2A::Read:   Unaligned read!   address = 0x%x,  size = %u\n", addr, size);
        return eng.ReadUnaligned(addr - eng.GetOffset(), size);
    }

    log_spew("NV2A::Read:   Unmapped read!   address = 0x%x,  size = %u\n", addr, size);
    return 0;
}

void NV2A::Write(const uint32_t addr, const uint32_t value, const uint8_t size) {
    auto opt_eng = FindEngine(addr);
    if (opt_eng) {
        auto& eng = opt_eng->get();
        // Aligned 32-bit wrute as expected
        if ((addr & 3) == 0 && size == 4) {
            eng.Write(addr - eng.GetOffset(), value);
        }
        else {
            // Unaligned or non 32-bit write
            eng.WriteUnaligned(addr - eng.GetOffset(), value, size);
        }
    }
    else {
        log_spew("NV2A::Write:  Unmapped write!  address = 0x%x,  value = 0x%x,  size = %u\n", addr, value, size);
    }
}

void NV2A::RegisterEngine(nv2a::NV2AEngine& engine) {
    engines.insert({ engine.GetOffset() + engine.GetLength() - 1, engine });
}

std::optional<std::reference_wrapper<nv2a::NV2AEngine>> NV2A::FindEngine(const uint32_t address) {
    auto entry = engines.lower_bound(address);
    if (entry != engines.end()) {
        auto& engine = entry->second;
        if (engine.Contains(address)) {
            return engine;
        }
    }
    return std::nullopt;
}

}
