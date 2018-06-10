/*
 * Portions of the code are based on Cxbx-Reloaded's OHCI LLE implementation.
 * The original copyright header is included below.
 */
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Cxbx.h
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include <cstdint>

namespace openxbox {

/* This is a linux struct for vectored I/O. See readv() and writev() */
typedef struct _IoVec {
    void* Iov_Base;  // Starting address
    size_t Iov_Len;  // Number of bytes to transfer
}
IoVec;

typedef struct _IOVector {
    IoVec* IoVecStruct;
    int IoVecNumber;      // number of I/O buffers supplied
    int AllocNumber;      // number of IoVec structs currently allocated
    size_t Size;          // total size of all I/O buffers supplied
}
IOVector;

inline uint64_t Muldiv64(uint64_t a, uint32_t b, uint32_t c);

void IoVecReset(IOVector* qiov);
void IoVecAdd(IOVector* qiov, void* base, size_t len);
size_t IoVecTobuffer(const IoVec* iov, const unsigned int iov_cnt, size_t offset, void *buf, size_t bytes);
size_t IoVecFromBuffer(const IoVec* iov, unsigned int iov_cnt, size_t offset, void* buf, size_t bytes);

}
