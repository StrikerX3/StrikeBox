file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/vixen/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vixen/*.cpp
    )

set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    )

if (WIN32)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vixen/win32")
endif ()
if (UNIX)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vixen/linux")
endif ()

# Add Visual Studio filters to better organize the code
vs_set_filters("${SOURCES}" "vixen")

# Main Executable
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions("-Wall -Werror -O0 -g -fPIC")
endif()

add_library(common STATIC ${SOURCES})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Include ld
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(common ${CMAKE_DL_LIBS})
endif()

# Make the Debug and RelWithDebInfo targets use Program Database for Edit and Continue for easier debugging
vs_use_edit_and_continue()

