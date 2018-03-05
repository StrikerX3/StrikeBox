file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/openxbox/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/openxbox/*.cpp
    )

set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    )

if (WIN32)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/openxbox/win32")
endif (WIN32)

# Add Visual Studio filters to better organize the code
vs_set_filters("${SOURCES}" "openxbox")

# Main Executable
if(NOT MSVC)
    add_definitions("-Wall -Werror -O0 -g")
endif()
add_library(common STATIC ${SOURCES})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Make the Debug and RelWithDebInfo targets use Program Database for Edit and Continue for easier debugging
vs_use_edit_and_continue()
