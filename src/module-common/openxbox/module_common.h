#pragma once

namespace openxbox {
namespace modules {

// Shared library export/import specs
#ifdef _WIN32
	#if defined(MODULE_EXPORTS)
		#define MODULE_API __declspec(dllexport)
	#else
		#define MODULE_API __declspec(dllimport)
	#endif
#else
	#define MODULE_API
#endif


#ifdef _WIN32
	#define MODULE_EXPORT __declspec(dllexport)
#else
	#define MODULE_EXPORT
#endif


// Module types
enum Type {
	TYPE_CPU,
};

// The API version type
typedef unsigned long APIVersion;

// The common module API version
// Increment this if there are any ABI breaking changes
const APIVersion apiVersion = 1;

// Base module information exposed to the emulator core.
// This struct MUST remain backwards-compatible -- existing fields MUST NOT be
// modified, moved or removed.
struct Info {
    APIVersion apiVersion;
	Type type;
};

// Field values for the above struct.
// Every derived module struct must define these values
#define COMMON_MODULE_INFO(type) {\
	apiVersion, \
	type \
}

}
}
