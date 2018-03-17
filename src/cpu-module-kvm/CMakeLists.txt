# Add sources
file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/jvm/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kvm/*.cpp
    )

set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    )


# Export OpenXBOX module
add_definitions(-DOPENXBOX_MODULE_EXPORTS)

# Main Executable
add_definitions("-Wall -Werror -O0 -g")
add_library(cpu-module-kvm SHARED "${SOURCES}")
target_include_directories(cpu-module-kvm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Include OpenXBOX common module code
target_link_libraries(cpu-module-kvm common cpu-module)

# Copy SO to CLI output directory
string(TOLOWER ${CPU_MODULE} CPU_MODULE_LC)
if(CPU_MODULE_LC STREQUAL kvm)
    add_custom_command(TARGET cpu-module-kvm
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/cli/modules
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/*.so ${CMAKE_BINARY_DIR}/src/cli/modules
        COMMENT "Copy DLLs to target directory")
endif()

