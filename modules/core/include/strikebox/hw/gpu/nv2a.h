// StrikeBox NV2A emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// NV2A is a custom GPU designed by Nvidia for the Original Xbox. Internally,
// the chip is split up into different engines that are mapped to distinct
// regions of the system memory allocated to the device:
//
//  Engine       Offset      Length    Description
// ----------------------------------------------------------------------------
//  PMC        0x000000      0x1000    Master control
//  PBUS       0x001000      0x1000    Bus control
//  PFIFO      0x002000      0x2000    MMIO and DMA FIFO submission to PGRAPH  (there's no VPE in NV2A)
//  PRMA       0x007000      0x1000    Real mode BAR access
//  PVIDEO     0x008000      0x1000    Video overlay
//  PTIMER     0x009000      0x1000    Time measurement and time-based alarms
//  PCOUNTER   0x00A000      0x1000    Performance monitoring counters
//  PNVIO      0x0C0000      0x1000    VGA sequencer and graph controller registers
//  PFB        0x100000      0x1000    Memory interface
//  PSTRAPS    0x101000      0x1000    Straps readout
//  PROM       0x300000     0x20000    ROM access window
//  PGRAPH     0x400000      0x2000    2D/3D graphics engine
//  PCRTC      0x600000      0x1000    CRTC controls
//  PRMCIO     0x601000      0x1000    VGA CRTC and attribute controller registers
//  PRAMDAC    0x680000      0x1000    RAMDAC, video overlay, cursor, and PLL control
//  PRMDIO     0x681000      0x1000    VGA DAC registers
//  PRAMIN     0x700000    0x100000    RAMIN access
//  USER       0x800000    0x200000    PFIFO MMIO/DMA submission area
//
#pragma once

#include "state.h"
#include "engine.h"

#include "pmc.h"
#include "pbus.h"
#include "pfifo.h"
#include "prma.h"
#include "pvideo.h"
#include "ptimer.h"
#include "pcounter.h"
#include "pnvio.h"
#include "pfb.h"
#include "pstraps.h"
#include "prom.h"
#include "pgraph.h"
#include "pcrtc.h"
#include "prmcio.h"
#include "pramdac.h"
#include "prmdio.h"
#include "pramin.h"
#include "user.h"
