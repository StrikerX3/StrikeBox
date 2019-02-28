#pragma once

#include "strikebox/xbox.h"

namespace strikebox {

struct StrikeBoxInfo {
	const char *version;
	const uint64_t versionId;
};

const struct StrikeBoxInfo * GetStrikeBoxInfo();

}
