#pragma once

#include <cstdint>

#include "defs.h"
#include "sm.h"

namespace openxbox {

// Xbox TV encoder chips.
typedef enum {
	// http://xboxdevwiki.net/Hardware_Revisions#Video_encoder
	Conexant,
	Focus,
	XCalibur
} TVEncoder;


TVEncoder TVEncoderFromHardwareModel(HardwareModel hardwareModel);

class TVEncoderDevice : public SMDevice {

};

}
