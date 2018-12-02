# Add sources
file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/whvp/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/whvp/*.cpp
    )

set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    )


# Export module
add_definitions(-DMODULE_EXPORTS)

# Add Visual Studio filters to better organize the code
vs_set_filters("${SOURCES}")

# Main Executable
if(NOT MSVC)
    add_definitions("-Wall -Werror -O0 -g")
endif()
add_library(cpu-module-whvp SHARED "${SOURCES}")
target_include_directories(cpu-module-whvp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Include common module code
target_link_libraries(cpu-module-whvp common cpu-module)

# Make the Debug and RelWithDebInfo targets use Program Database for Edit and Continue for easier debugging
vs_use_edit_and_continue()

# Copy DLL to CLI output directory
string(TOLOWER ${CPU_MODULE} CPU_MODULE_LC)
if(CPU_MODULE_LC STREQUAL whvp)
    if(MSVC)
        add_custom_command(TARGET cpu-module-whvp
            POST_BUILD
            COMMAND if not exist \"$(ProjectDir)..\\cli\\$(Configuration)\\modules\" mkdir \"$(ProjectDir)..\\cli\\$(Configuration)\\modules\"
            COMMAND copy /b /y \"$(TargetDir)*.dll\" \"$(ProjectDir)..\\cli\\$(Configuration)\\modules\"
            COMMENT "Copy DLLs to target directory")
    endif()
endif()

