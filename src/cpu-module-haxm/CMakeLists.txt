# Add sources
file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/haxm/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/haxm/*.cpp
    )

set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    )


# Export OpenXBOX module
add_definitions(-DOPENXBOX_MODULE_EXPORTS)

# Add Visual Studio filters to better organize the code
vs_set_filters("${SOURCES}")

# Main Executable
if(NOT MSVC)
    add_definitions("-Wall -Werror -O0 -g")
endif()
add_library(cpu-module-haxm SHARED "${SOURCES}")
target_include_directories(cpu-module-haxm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Include OpenXBOX common module code
target_link_libraries(cpu-module-haxm common cpu-module)

# Make the Debug and RelWithDebInfo targets use Program Database for Edit and Continue for easier debugging
vs_use_edit_and_continue()

# Copy DLL to CLI output directory
string(TOLOWER ${CPU_MODULE} CPU_MODULE_LC)
if(CPU_MODULE_LC STREQUAL haxm)
    if(MSVC)
        add_custom_command(TARGET cpu-module-haxm
            POST_BUILD
            COMMAND if not exist \"$(ProjectDir)..\\cli\\$(Configuration)\\modules\" mkdir \"$(ProjectDir)..\\cli\\$(Configuration)\\modules\"
            COMMAND copy /b /y \"$(TargetDir)*.dll\" \"$(ProjectDir)..\\cli\\$(Configuration)\\modules\"
            COMMENT "Copy DLLs to target directory")
    endif()
endif()
