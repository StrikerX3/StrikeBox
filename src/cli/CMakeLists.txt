# Add sources
file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    )

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Add Visual Studio filters to better organize the code
vs_set_filters("${SOURCES}")

# Main Executable
if(NOT MSVC)
    add_definitions("-Wall -Werror -O0 -g")
endif()
add_executable(cli ${SOURCES})
target_include_directories(cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Include OpenXBOX Core
target_link_libraries(cli core)

# Name the output file "openxbox-cli"
set_target_properties(cli PROPERTIES OUTPUT_NAME "openxbox-cli")

# Make the Debug and RelWithDebInfo targets use Program Database for Edit and Continue for easier debugging
vs_use_edit_and_continue()

# Set the working directory to the path where the executable file is deployed
if(MSVC)
    set_target_properties(cli PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
endif()

# Add custom build commands to copy modules to the command line front-end build output directory
if(MSVC)
    set(CPU_MODULE "none" CACHE STRING "Choose a CPU module to use: haxm, whvp, none")

    # Create modules directory
    add_custom_command(TARGET cli
        POST_BUILD
        COMMAND if not exist \"$(TargetDir)modules\" mkdir \"$(TargetDir)modules\")

    # Include libraries and copy module DLLs according to user option
    string(TOLOWER ${CPU_MODULE} CPU_MODULE_LC)
    if(CPU_MODULE_LC STREQUAL haxm)
        message(STATUS "CLI front-end will use Intel HAXM CPU module")
        target_link_libraries(cli cpu-module-haxm)
    elseif(CPU_MODULE_LC STREQUAL whvp)
        message(STATUS "CLI front-end will use Windows Hypervisor Platform CPU module")
        target_link_libraries(cli cpu-module-whvp)
    elseif(CPU_MODULE_LC STREQUAL none)
        message(WARNING "No CPU module specified. OpenXBOX requires at least one CPU module to run. "
            "Make sure to add one to the module subdirectory in the build output directory, or set the CPU_MODULE option to one of the available options.")
    else()
        message(SEND_ERROR "Invalid CPU module specified. Check your CPU_MODULE option.")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CPU_MODULE "none" CACHE STRING "Choose a CPU module to use: kvm, none")

    add_custom_command(TARGET cli
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/cli/modules)

    # Include libraries according to user option
    string(TOLOWER ${CPU_MODULE} CPU_MODULE_LC)
    if(CPU_MODULE_LC STREQUAL kvm)
        message(STATUS "CLI front-end will use KVM CPU module")
        target_link_libraries(cli cpu-module-kvm)
    elseif(CPU_MODULE_LC STREQUAL none)
        message(WARNING "No CPU module specified. OpenXBOX requires at least one CPU module to run. "
            "Make sure to add one to the module subdirectory in the build output directory, or set the CPU_MODULE option to one of the available options.")
    else()
        message(SEND_ERROR "Invalid CPU module specified. Check your CPU_MODULE option.")
    endif()
endif()

# Include additional libraries on GCC
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    find_package(Threads REQUIRED)
    target_link_libraries(cli -lstdc++fs ${CMAKE_THREAD_LIBS_INIT})
endif()
