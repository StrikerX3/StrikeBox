file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/vixen/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vixen/*.cpp
    )

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vixen/hw")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vixen/util")

set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    )

include("${CMAKE_SOURCE_DIR}/cmake/PrecompiledHeader.cmake")
include(ExternalProject)

# Add Visual Studio filters to better organize the code
vs_set_filters("${SOURCES}" "vixen")

# Main Executable
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions("-Wall -Werror -O0 -g")
endif()
add_library(core STATIC
	${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/vixen/pch.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vixen/pch.h")

# Use precompiled headers to speed up compilation
add_precompiled_header(core vixen/pch.h FORCEINCLUDE SOURCE_CXX "${CMAKE_CURRENT_SOURCE_DIR}/vixen/pch.cpp")

# Include Zydis
add_definitions(-DZYDIS_STATIC_DEFINE)
ExternalProject_Add(
    Zydis
    GIT_REPOSITORY "https://github.com/zyantific/zydis"
    GIT_TAG "v2.0.0"
    UPDATE_COMMAND ""
    INSTALL_COMMAND "")
ExternalProject_Get_Property(Zydis install_dir)
include_directories(${install_dir}/src/Zydis/include ${install_dir}/src/Zydis-build)
add_dependencies(core Zydis)

if(WIN32)
	target_link_libraries(core ${install_dir}/src/Zydis-build/${CMAKE_CFG_INTDIR}/Zydis.lib)
else()
	target_link_libraries(core ${install_dir}/src/Zydis-build/libZydis.a)
endif()

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${install_dir}/src/Zydis/include)

# Include viXen module interfaces
target_link_libraries(core common cpu-module)

# Make the Debug and RelWithDebInfo targets use Program Database for Edit and Continue for easier debugging
vs_use_edit_and_continue()

