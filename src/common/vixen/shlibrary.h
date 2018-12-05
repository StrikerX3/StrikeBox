#pragma once

#include <string>
#include <stdint.h>

namespace vixen {

// ----- Library load status --------------------------------------------------

/*!
 * Indicates the result of attempting to load a library.
 */
typedef int32_t SharedLibraryStatus;

// The shared library was loaded successfully
const SharedLibraryStatus kLibraryLoadSuccess = 0x00000000;

// The shared library could not be found at the specified path
const SharedLibraryStatus kLibraryNotFound = 0x80000001;

// The file specified is not a valid shared library image
const SharedLibraryStatus kLibraryInvalidImage = 0x80000002;

// An unspecified error occurred while trying to load the shared library
const SharedLibraryStatus kLibraryUnspecifiedError = 0xFFFFFFFF;

// ----- Library class --------------------------------------------------------

/*!
 * A shared library loaded into the application.
 */
class SharedLibrary {
public:
	virtual ~SharedLibrary();

	virtual void *GetExport(std::string exportName) = 0;
};

// ----- Library functions ----------------------------------------------------

/*!
 * Loads a shared library from the specified path.
 * Returns nullptr if the library fails to load.
 */
SharedLibraryStatus SharedLibrary_Load(std::wstring path, SharedLibrary **library);


}
