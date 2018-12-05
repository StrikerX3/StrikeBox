#pragma once

#include "vixen/xbox.h"
#include "vixen/module.h"

namespace vixen {

struct viXenInfo {
	const char *version;
	const uint64_t versionId;
};

const struct viXenInfo * GetViXenInfo();

}
