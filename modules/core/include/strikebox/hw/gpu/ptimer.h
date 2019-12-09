// StrikeBox NV2A PTIMER (Time measurement and time-based alarms) engine emulation
// (C) Ivan "StrikerX3" Oliveira
//
// Based on envytools:
// https://envytools.readthedocs.io/en/latest/index.html
//
// References to particular items in the documentation are denoted between
// brackets optionally followed by a quote from the documentation.
//
// [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html]
// "PTIMER is a small functional unit used to measure time by the card.
//  It has a 56-bit tick counter connected to a programmable clock source.
//  The current value of this counter is used for timestamping by many other units on the GPU.
//  Two such timestamps can be substracted to get the wall time elapsed between their creation and measure eg. command execution time.
//  Also, it's possible to set up an interrupt that will be triggered when the low 27 bits of the counter reach a specified value."
//
// PTIMER engine registers occupy the range 0x009000..0x009FFF.
#pragma once

#include "engine.h"

#include <chrono>

namespace strikebox::nv2a {

// PTIMER registers
// [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html#mmio-register-list-nv3]
const uint32_t Reg_PTIMER_INTR        = 0x100; // [RW] Interrupt status                    [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html#the-alarm-and-interrupts]
const uint32_t Reg_PTIMER_INTR_ENABLE = 0x140; // [RW] Interrupt enable                    [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html#the-alarm-and-interrupts]
/**/const uint32_t Val_PTIMER_INTR_ALARM = (1 << 0); // bit 0: alarm
const uint32_t Reg_PTIMER_CLOCK_DIV   = 0x200; // [RW] Clock divider                       [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html#the-clock-ratio]
const uint32_t Reg_PTIMER_CLOCK_MUL   = 0x210; // [RW] Clock multiplier                    [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html#the-clock-ratio]
const uint32_t Reg_PTIMER_TIME_LOW    = 0x400; // [R ] Low part of the time counter        [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html#the-time-counter]
const uint32_t Reg_PTIMER_TIME_HIGH   = 0x410; // [R ] High part of the time counter       [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html#the-time-counter]
const uint32_t Reg_PTIMER_ALARM       = 0x420; // [RW] The TIME_LOW value to interrupt on  [https://envytools.readthedocs.io/en/latest/hw/bus/ptimer.html#the-alarm-and-interrupts]

// ----------------------------------------------------------------------------

// NV2A time measurement and time-based alarms (PTIMER)
class PTIMER : public NV2AEngine {
public:
    PTIMER(NV2A& nv2a) : NV2AEngine("PTIMER", 0x009000, 0x1000, nv2a) {}

    void SetEnabled(bool enabled);

    void Reset() override;
    uint32_t Read(const uint32_t addr) override;
    void Write(const uint32_t addr, const uint32_t value) override;

    bool GetInterruptState() { return m_interruptLevels & m_enabledInterrupts; }

private:
    bool m_enabled = false;

    uint32_t m_interruptLevels;
    uint32_t m_enabledInterrupts;

    uint32_t m_clockMul;
    uint32_t m_clockDiv;
    uint32_t m_alarm;

    uint64_t GetTickCount();
    uint64_t m_tickCount;
    std::chrono::high_resolution_clock::time_point m_lastTickCountRead;
};

}
