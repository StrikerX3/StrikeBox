/*
 * Portions of the code are based on QEMU's ISA bus support.
 * The original copyright header is included below.
 */
/*
 * isa bus support for qdev.
 *
 * Copyright (c) 2009 Gerd Hoffmann <kraxel@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include "isabus.h"
#include "openxbox/log.h"

namespace openxbox {

ISABus::ISABus() {
    m_irqs = nullptr;
}

ISABus::~ISABus() {
}

void ISABus::ConfigureIRQs(IRQ* irqs) {
    m_irqs = irqs;
}

IRQ *ISABus::GetIRQ(uint8_t isaIRQ) {
    if (isaIRQ > 15) {
        log_warning("ISABus::GetIRQ: invalid ISA IRQ %u\n", isaIRQ);
        return nullptr;
    }
    return &m_irqs[isaIRQ];
}

}
