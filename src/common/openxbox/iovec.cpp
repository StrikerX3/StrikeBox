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
#include "iovec.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

namespace openxbox {

#pragma warning(pop)

void IoVecReset(IOVector* qiov) {
    assert(qiov->AllocNumber != -1);

    qiov->IoVecNumber = 0;
    qiov->Size = 0;
}

void IoVecAdd(IOVector* qiov, void* base, size_t len) {
    assert(qiov->AllocNumber != -1);

    if (qiov->IoVecNumber == qiov->AllocNumber) {
        qiov->AllocNumber = 2 * qiov->AllocNumber + 1;
        qiov->IoVecStruct = static_cast<IoVec*>(std::realloc(qiov->IoVecStruct, qiov->AllocNumber * sizeof(IOVector)));
    }
    qiov->IoVecStruct[qiov->IoVecNumber].Iov_Base = base;
    qiov->IoVecStruct[qiov->IoVecNumber].Iov_Len = len;
    qiov->Size += len;
    ++qiov->IoVecNumber;
}

// This takes "iov_cnt" of "iov" buffers as input and copies sequentially their contents to the "buf" output buffer.
// "offset" indicates the offset inside "bytes" (total lenght of "iov" buffers) where the copy is going to start.
// "offset" must be less than "bytes" or else the assertion will fail. "done" is the number of bytes actually copied
size_t IoVecTobuffer(const IoVec* iov, const unsigned int iov_cnt, size_t offset, void* buf, size_t bytes) {
    size_t done;
    unsigned int i;
    for (i = 0, done = 0; (offset || done < bytes) && i < iov_cnt; i++) {
        if (offset < iov[i].Iov_Len) {
            size_t len = MIN(iov[i].Iov_Len - offset, bytes - done);
            std::memcpy(static_cast<uint8_t*>(buf) + done, static_cast<uint8_t*>(iov[i].Iov_Base) + offset, len);
            done += len;
            offset = 0;
        }
        else {
            offset -= iov[i].Iov_Len;
        }
    }
    assert(offset == 0);
    return done;
}

// This does the opposite of IoVecTobuffer: it takes "buf" as input and copies sequentially its contents to the
// "iov" output buffers.
size_t IoVecFromBuffer(const IoVec* iov, unsigned int iov_cnt, size_t offset, void* buf, size_t bytes) {
    size_t done;
    unsigned int i;
    for (i = 0, done = 0; (offset || done < bytes) && i < iov_cnt; i++) {
        if (offset < iov[i].Iov_Len) {
            size_t len = MIN(iov[i].Iov_Len - offset, bytes - done);
            memcpy(static_cast<uint8_t*>(iov[i].Iov_Base) + offset, static_cast<uint8_t*>(buf) + done, len);
            done += len;
            offset = 0;
        }
        else {
            offset -= iov[i].Iov_Len;
        }
    }
    assert(offset == 0);
    return done;
}

}
