if(MSVC)
    cmake_minimum_required (VERSION 3.8)
else()
    cmake_minimum_required (VERSION 3.1)
endif()

# Differentiate between Linux and macOS
if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

project (vixen)

if(MSVC)
	# Set platform names
	if("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
		set(BUILD_PLATFORM win64)
	else()
		set(BUILD_PLATFORM win32)
	endif()

	# Use folders and set the startup project
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT cli)
endif()


include("${CMAKE_SOURCE_DIR}/cmake/VSHelpers.cmake")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Require C++11 features
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler and linker flags except on MSVC
if(NOT MSVC)
	add_definitions("-Wall -Werror -O0 -g")
endif()

# Add viXen projects
add_subdirectory("${CMAKE_SOURCE_DIR}/src/common")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/module-common")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/cpu-module")
if(WIN32)
add_subdirectory("${CMAKE_SOURCE_DIR}/src/cpu-module-haxm")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/cpu-module-whvp")
endif()
if(LINUX)
add_subdirectory("${CMAKE_SOURCE_DIR}/src/cpu-module-kvm")
endif()
add_subdirectory("${CMAKE_SOURCE_DIR}/src/core")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/cli")

