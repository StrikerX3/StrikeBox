#pragma once

#include "vixen/xbox.h"

namespace vixen {

struct viXenInfo {
	const char *version;
	const uint64_t versionId;
};

const struct viXenInfo * GetViXenInfo();

}
