// ATA/ATAPI-4 emulation for the Original Xbox
// (C) Ivan "StrikerX3" Oliveira
//
// This code aims to implement a subset of the ATA/ATAPI-4 specification
// that satisifies the requirements of an IDE interface for the Original Xbox.
//
// Specification:
// http://www.t13.org/documents/UploadedDocuments/project/d1153r18-ATA-ATAPI-4.pdf
//
// References to particular items in the specification are denoted between brackets
// optionally followed by a quote from the specification.
#include "ata_common.h"

namespace vixen {
namespace hw {
namespace ata {

void ATARegisters::WriteSignature(bool packetFeatureSet) {
    // Write signature according to Signature and Persistence protocol [9.1]
    if (packetFeatureSet) {
        sectorCount = 0x01;
        sectorNumber = 0x01;
        cylinder = 0xEB14;
        deviceHead = 0x10;
    }
    else {
        sectorCount = 0x01;
        sectorNumber = 0x01;
        cylinder = 0x0000;
        deviceHead = 0x00;
    }
}

}
}
}
