#pragma once

#include <string>
#include <map>
#include <optional>

#include "pci.h"
#include "../basic/irq.h"
#include "../gpu/nv2a.h"

namespace strikebox {

class NV2ADevice : public PCIDevice {
public:
    NV2ADevice(uint8_t *pSystemRAM, uint32_t systemRAMSize, IRQHandler& irqHandler);
    virtual ~NV2ADevice();

    // PCI Device functions
    void Init();
    void Reset();
    
    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;
    void PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) override;
    void PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) override;

private:
    uint8_t *m_pSystemRAM;
    uint32_t m_systemRAMSize;
    IRQHandler& m_irqHandler;

    // NV2A state and engines
    nv2a::NV2A      m_nv2a;
    nv2a::PMC       m_pmc     { m_nv2a };
    nv2a::PBUS      m_pbus    { m_nv2a };
    nv2a::PFIFO     m_pfifo   { m_nv2a };
    nv2a::PRMA      m_prma    { m_nv2a };
    nv2a::PVIDEO    m_pvideo  { m_nv2a };
    nv2a::PTIMER    m_ptimer  { m_nv2a };
    nv2a::PCOUNTER  m_pcounter{ m_nv2a };
    nv2a::PNVIO     m_pnvio   { m_nv2a };
    nv2a::PFB       m_pfb     { m_nv2a };
    nv2a::PSTRAPS   m_pstraps { m_nv2a };
    nv2a::PROM      m_prom    { m_nv2a };
    nv2a::PGRAPH    m_pgraph  { m_nv2a };
    nv2a::PCRTC     m_pcrtc   { m_nv2a };
    nv2a::PRMCIO    m_prmcio  { m_nv2a };
    nv2a::PRAMDAC   m_pramdac { m_nv2a };
    nv2a::PRMDIO    m_prmdio  { m_nv2a };
    nv2a::PRAMIN    m_pramin  { m_nv2a };
    nv2a::USER      m_user    { m_nv2a };

    // Fast engine lookup
    std::map<uint32_t, nv2a::NV2AEngine&> engines;
    void RegisterEngine(nv2a::NV2AEngine& engine);
    std::optional<std::reference_wrapper<nv2a::NV2AEngine>> FindEngine(const uint32_t address);
};

}
