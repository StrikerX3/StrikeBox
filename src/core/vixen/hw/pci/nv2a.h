#pragma once

#include <cstdint>

#include "../defs.h"
#include "pci.h"
#include "../nv2a/defs.h"
#include "../nv2a/vga.h"
#include "../basic/irq.h"

namespace vixen {

class NV2ADevice : public PCIDevice {
public:
    NV2ADevice(uint8_t *pSystemRAM, uint32_t systemRAMSize, IRQHandler *irqHandler);
    
    virtual ~NV2ADevice();

    // PCI Device functions
    void Init();
    void Reset();

    void PCIIORead(int barIndex, uint32_t port, uint32_t *value, uint8_t size) override;
    void PCIIOWrite(int barIndex, uint32_t port, uint32_t value, uint8_t size) override;
    void PCIMMIORead(int barIndex, uint32_t addr, uint32_t *value, uint8_t size) override;
    void PCIMMIOWrite(int barIndex, uint32_t addr, uint32_t value, uint8_t size) override;

private:
    const NV2ABlockInfo* FindBlock(uint32_t addr);

    static void PMCRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PMCWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PBUSRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PBUSWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PFIFORead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PFIFOWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PRMARead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PRMAWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PVIDEORead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PVIDEOWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PTIMERRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PTIMERWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PCOUNTERRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PCOUNTERWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PVPERead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PVPEWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PTVRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PTVWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PRMFBRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PRMFBWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PRMVIORead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PRMVIOWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PFBRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PFBWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PSTRAPSRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PSTRAPSWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PGRAPHRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PGRAPHWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PCRTCRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PCRTCWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PRMCIORead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PRMCIOWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PRAMDACRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PRAMDACWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PRMDIORead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PRMDIOWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void PRAMINRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void PRAMINWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    static void USERRead(NV2ADevice* pNV2A, uint32_t addr, uint32_t *value, uint8_t size);
    static void USERWrite(NV2ADevice* pNV2A, uint32_t addr, uint32_t value, uint8_t size);

    uint32_t ramht_hash(uint32_t handle);
    RAMHTEntry ramht_lookup(uint32_t handle);

    uint32_t ptimer_get_clock();

    void pgraph_set_context_user(uint32_t value);
    void pgraph_context_switch(unsigned int channel_id);
    void pgraph_wait_fifo_access();
    void pgraph_method_log(unsigned int subchannel, unsigned int graphics_class, unsigned int method, uint32_t parameter);
    void pgraph_method(unsigned int subchannel, unsigned int method, uint32_t parameter);
    bool pgraph_color_write_enabled();
    bool pgraph_zeta_write_enabled();

    unsigned int kelvin_map_stencil_op(uint32_t parameter);
    unsigned int kelvin_map_polygon_mode(uint32_t parameter);
    unsigned int kelvin_map_texgen(uint32_t parameter, unsigned int channel);

    void load_graphics_object(uint32_t instance_address, GraphicsObject *obj);
    GraphicsObject* lookup_graphics_object(uint32_t instance_address);

    DMAObject nv_dma_load(uint32_t dma_obj_address);
    void *nv_dma_map(uint32_t dma_obj_address, uint32_t *len);

    void pfifo_run_pusher();

    static void PFIFO_Puller_Thread(NV2ADevice* pNV2a);
    static void VBlankThread(NV2ADevice* pNV2A);

    void UpdateIRQ();

    uint8_t *m_pSystemRAM;
    uint32_t m_systemRAMSize;
    IRQHandler *m_irqHandler;

    uint8_t* m_pRAMIN = nullptr;
    uint8_t* m_VRAM = nullptr;
    NV2APRAMDAC m_PRAMDAC;
    NV2APTIMER m_PTIMER;
    NV2APVIDEO m_PVIDEO;
    NV2APMC m_PMC;
    NV2APCRTC m_PCRTC;
    NV2APFIFO m_PFIFO;
    NV2APFB m_PFB;
    NV2APGRAPH m_PGRAPH;
    NV2APRMCIO m_PRMCIO;
    NV2AUSER m_User;

    VGACommonState m_VGAState;

    bool m_running;
    std::vector<NV2ABlockInfo> m_MemoryRegions;
    std::thread m_VblankThread;
};

}
