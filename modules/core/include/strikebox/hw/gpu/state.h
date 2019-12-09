// StrikeBox NV2A state class
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
#pragma once

#include <string>
#include <map>
#include <optional>
#include <functional>

#include "engine.h"

#include "engines/pmc.h"
#include "engines/pbus.h"
#include "engines/pfifo.h"
#include "engines/prma.h"
#include "engines/pvideo.h"
#include "engines/ptimer.h"
#include "engines/pcounter.h"
#include "engines/pnvio.h"
#include "engines/pfb.h"
#include "engines/pstraps.h"
#include "engines/prom.h"
#include "engines/pgraph.h"
#include "engines/pcrtc.h"
#include "engines/prmcio.h"
#include "engines/pramdac.h"
#include "engines/prmdio.h"
#include "engines/pramin.h"
#include "engines/user.h"

namespace strikebox::nv2a {

using PCIConfigReader = std::function<uint32_t(uint8_t address)>;
using PCIConfigWriter = std::function<void(uint8_t address, uint32_t value)>;
using IRQHandlerFunc = std::function<void(bool level)>;

// Represents the state of the NV2A GPU.
class NV2A {
public:
    NV2A(uint8_t* systemRAM, uint32_t systemRAMSize, PCIConfigReader readPCIConfig, PCIConfigWriter writePCIConfig, IRQHandlerFunc handleIRQ);

    // PCI config space read/write access
    const PCIConfigReader readPCIConfig = [](uint8_t) -> uint32_t { return 0; };
    const PCIConfigWriter writePCIConfig = [](uint8_t, uint32_t) {};

    // IRQ handler
    const IRQHandlerFunc handleIRQ = [](bool) {};

    // Shared system memory
    uint8_t* systemRAM = nullptr;
    const uint32_t systemRAMSize = 0;

    // NV2A engines
    std::unique_ptr<PMC>       pmc      = std::make_unique<PMC>(*this);
    std::unique_ptr<PBUS>      pbus     = std::make_unique<PBUS>(*this);
    std::unique_ptr<PFIFO>     pfifo    = std::make_unique<PFIFO>(*this);
    std::unique_ptr<PRMA>      prma     = std::make_unique<PRMA>(*this);
    std::unique_ptr<PVIDEO>    pvideo   = std::make_unique<PVIDEO>(*this);
    std::unique_ptr<PTIMER>    ptimer   = std::make_unique<PTIMER>(*this);
    std::unique_ptr<PCOUNTER>  pcounter = std::make_unique<PCOUNTER>(*this);
    std::unique_ptr<PNVIO>     pnvio    = std::make_unique<PNVIO>(*this);
    std::unique_ptr<PFB>       pfb      = std::make_unique<PFB>(*this);
    std::unique_ptr<PSTRAPS>   pstraps  = std::make_unique<PSTRAPS>(*this);
    std::unique_ptr<PROM>      prom     = std::make_unique<PROM>(*this);
    std::unique_ptr<PGRAPH>    pgraph   = std::make_unique<PGRAPH>(*this);
    std::unique_ptr<PCRTC>     pcrtc    = std::make_unique<PCRTC>(*this);
    std::unique_ptr<PRMCIO>    prmcio   = std::make_unique<PRMCIO>(*this);
    std::unique_ptr<PRAMDAC>   pramdac  = std::make_unique<PRAMDAC>(*this);
    std::unique_ptr<PRMDIO>    prmdio   = std::make_unique<PRMDIO>(*this);
    std::unique_ptr<PRAMIN>    pramin   = std::make_unique<PRAMIN>(*this);
    std::unique_ptr<USER>      user     = std::make_unique<USER>(*this);

    void Reset();
    uint32_t Read(const uint32_t addr, const uint8_t size);
    void Write(const uint32_t addr, const uint32_t value, const uint8_t size);

    inline void UpdateIRQ() { pmc->UpdateIRQ(); }

private:
    // Fast engine lookup
    std::map<uint32_t, nv2a::NV2AEngine&> engines;
    void RegisterEngine(nv2a::NV2AEngine& engine);
    std::optional<std::reference_wrapper<nv2a::NV2AEngine>> FindEngine(const uint32_t address);
};

}
