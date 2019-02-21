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
#include "vixen/hw/bus/isabus.h"

#include "vixen/log.h"

namespace vixen {

ISABus::ISABus(IRQ *irqs)
    : m_irqs(irqs)
{
}

ISABus::~ISABus() {
}

IRQ *ISABus::GetIRQ(uint8_t isaIRQ) {
    if (isaIRQ > 15) {
        log_warning("ISABus::GetIRQ: Invalid ISA IRQ %u\n", isaIRQ);
        return nullptr;
    }
    return &m_irqs[isaIRQ];
}

}
