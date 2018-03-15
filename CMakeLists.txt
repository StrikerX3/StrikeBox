if(MSVC)
    cmake_minimum_required (VERSION 3.8)
else()
    cmake_minimum_required (VERSION 2.6)
endif()

project (openxbox)

if(MSVC)
	# Set platform names
	if("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
		set(BUILD_PLATFORM win64)
		set(ZYDIS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/extern/zydis-2.0.0/include")
		set(ZYDIS_LIBRARY "${CMAKE_SOURCE_DIR}/extern/zydis-2.0.0/lib/Zydis.lib")
	else()
		set(BUILD_PLATFORM win32)
		set(ZYDIS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/extern/zydis-2.0.0/include")
		set(ZYDIS_LIBRARY "${CMAKE_SOURCE_DIR}/extern/zydis-2.0.0/lib/Zydis32.lib")
	endif()

	# Use folders and set the startup project
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT cli)
endif()

include("${CMAKE_SOURCE_DIR}/cmake/VSHelpers.cmake")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Add OpenXBOX projects
add_subdirectory("${CMAKE_SOURCE_DIR}/src/common")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/module-common")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/cpu-module")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/cpu-module-haxm")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/core")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/cli")