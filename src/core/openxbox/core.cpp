#include "openxbox/core.h"

#include <cstdint>

namespace openxbox {

#define OX_VERSION_MAJOR 0
#define OX_VERSION_MINOR 0
#define OX_VERSION_PATCH 1
#define STRFY(x) #x

// TODO: add other things:
// - git commit hash
// - build date
// - compiler name and version

#define MAKE_VERSION_STRING(major, minor, patch) (STRFY(major) "." STRFY(minor) "." STRFY(patch))
#define MAKE_VERSION_ID(major, minor, patch) ((((uint64_t)major) << 48ull) | (((uint64_t)minor) << 32ull) | ((uint64_t)patch))

static const OpenXBOXInfo g_OpenXBOXInfo = {
	MAKE_VERSION_STRING(OX_VERSION_MAJOR, OX_VERSION_MINOR, OX_VERSION_PATCH),
	MAKE_VERSION_ID(OX_VERSION_MAJOR, OX_VERSION_MINOR, OX_VERSION_PATCH)
};

const struct OpenXBOXInfo * GetOpenXBOXInfo() {
	return &g_OpenXBOXInfo;
}

}
