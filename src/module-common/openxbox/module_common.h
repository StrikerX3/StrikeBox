#pragma once

namespace openxbox {

// Shared library export/import specs
#ifdef _WIN32
	#if defined(OPENXBOX_MODULE_EXPORTS)
		#define OPENXBOX_MODULE_API __declspec(dllexport)
	#else
		#define OPENXBOX_MODULE_API __declspec(dllimport)
	#endif
#else
	#define OPENXBOX_MODULE_API
#endif


#ifdef _WIN32
	#define OPENXBOX_MODULE_EXPORT __declspec(dllexport)
#else
	#define OPENXBOX_MODULE_EXPORT
#endif


// OpenXBOX module types
enum OpenXBOXModuleType {
	OX_MODULE_CPU,
};

// The common module API version
// Increment this if there are any ABI breaking changes
#define OPENXBOX_MODULE_API_VERSION 1

// Base module information exposed to the emulator core.
// This struct MUST remain backwards-compatible -- existing fields MUST NOT be
// modified, moved or removed.
struct OpenXBOXModuleInfo {
	int moduleAPIVersion;
	OpenXBOXModuleType moduleType;
};

// Field values for the above struct.
// Every derived module struct must define these values
#define COMMON_OPENXBOX_MODULE_INFO(moduleType) {\
	OPENXBOX_MODULE_API_VERSION, \
	moduleType \
}

}