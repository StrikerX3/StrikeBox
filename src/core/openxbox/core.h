#pragma once

#include "openxbox/xbox.h"
#include "openxbox/module.h"

namespace openxbox {

struct OpenXBOXInfo {
	const char *version;
	const uint64_t versionId;
};

const struct OpenXBOXInfo * GetOpenXBOXInfo();

}
