file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/openxbox/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/openxbox/*.cpp
    )

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/openxbox/hw")

set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    )

include("${CMAKE_SOURCE_DIR}/cmake/PrecompiledHeader.cmake")

# Add Visual Studio filters to better organize the code
vs_set_filters("${SOURCES}" "openxbox")

# Main Executable
if(NOT MSVC)
    add_definitions("-Wall -Werror -O0 -g")
endif()
add_library(core STATIC
	${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/openxbox/pch.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/openxbox/pch.h")

# Use precompiled headers to speed up compilation
add_precompiled_header(core openxbox/pch.h FORCEINCLUDE SOURCE_CXX "${CMAKE_CURRENT_SOURCE_DIR}/openxbox/pch.cpp")

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Include OpenXBOX module interfaces
target_link_libraries(core common cpu-module)

# Make the Debug and RelWithDebInfo targets use Program Database for Edit and Continue for easier debugging
vs_use_edit_and_continue()
