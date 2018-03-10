#include "tvenc.h"

namespace openxbox {

TVEncoder TVEncoderFromHardwareModel(HardwareModel hardwareModel) {
    switch (hardwareModel) {
    case Revision1_0:
    case Revision1_1:
    case Revision1_2:
    case Revision1_3:
        return TVEncoder::Conexant;
    case Revision1_4:
        return TVEncoder::Focus;
    case Revision1_5:
        return TVEncoder::Focus; // Assumption
    case Revision1_6:
        return TVEncoder::XCalibur;
    case DebugKit:
        // LukeUsher : My debug kit and at least most of them (maybe all?)
        // are equivalent to v1.0 and have Conexant encoders.
        return TVEncoder::Conexant;
    default:
        // UNREACHABLE(hardwareModel);
        return TVEncoder::Focus;
    }
}

}
