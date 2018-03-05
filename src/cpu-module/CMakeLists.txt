# Add sources
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

# Add Visual Studio filters to better organize the code
vs_set_filters("${SOURCES}" "openxbox")

# Main Executable
if(NOT MSVC)
    add_definitions("-Wall -Werror -O0 -g")
endif()
add_library(cpu-module STATIC "${SOURCES}")
target_include_directories(cpu-module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Include OpenXBOX common module code
target_link_libraries(cpu-module module-common)

# Make the Debug and RelWithDebInfo targets use Program Database for Edit and Continue for easier debugging
vs_use_edit_and_continue()
