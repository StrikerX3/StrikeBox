#pragma once

#include <cstdint>

#include "defs.h"

namespace openxbox {

// Xbox TV encoder chips.
typedef enum {
	// http://xboxdevwiki.net/Hardware_Revisions#Video_encoder
	Conexant,
	Focus,
	XCalibur
} TVEncoder;


TVEncoder TVEncoderFromHardwareModel(HardwareModel hardwareModel);

}
