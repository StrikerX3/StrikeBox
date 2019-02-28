#include "strikebox/core.h"

#include <cstdint>

namespace strikebox {

#define VXN_VERSION_MAJOR 0
#define VXN_VERSION_MINOR 0
#define VXN_VERSION_PATCH 1
#define STRFY(x) #x

// TODO: add other things:
// - git commit hash
// - build date
// - compiler name and version

#define MAKE_VERSION_STRING(major, minor, patch) (STRFY(major) "." STRFY(minor) "." STRFY(patch))
#define MAKE_VERSION_ID(major, minor, patch) ((((uint64_t)major) << 48ull) | (((uint64_t)minor) << 32ull) | ((uint64_t)patch))

static const StrikeBoxInfo g_StrikeBoxInfo = {
	MAKE_VERSION_STRING(VXN_VERSION_MAJOR, VXN_VERSION_MINOR, VXN_VERSION_PATCH),
	MAKE_VERSION_ID(VXN_VERSION_MAJOR, VXN_VERSION_MINOR, VXN_VERSION_PATCH)
};

const struct StrikeBoxInfo * GetStrikeBoxInfo() {
	return &g_StrikeBoxInfo;
}

}
