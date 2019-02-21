project(vixen-cli VERSION 1.0.0 LANGUAGES CXX)

##############################
# Source files
#
file(GLOB_RECURSE sources
    src/*.cpp
)

file(GLOB_RECURSE private_headers
    src/*.hpp
    src/*.h
)

file(GLOB_RECURSE public_headers
    include/*.hpp
    include/*.h
)

##############################
# Project structure
#
add_executable(vixen-cli ${sources} ${private_headers} ${public_headers})

target_include_directories(vixen-cli
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(vixen-cli vixen-core)

if(MSVC)
    add_precompiled_header(vixen-cli pch.hpp SOURCE_CXX "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp" FORCEINCLUDE)

    vs_set_filters(BASE_DIR src FILTER_ROOT "Sources" SOURCES ${sources})
    vs_set_filters(BASE_DIR src FILTER_ROOT "Private Headers" SOURCES ${private_headers})
    vs_set_filters(BASE_DIR include FILTER_ROOT "Public Headers" SOURCES ${public_headers})

    vs_use_edit_and_continue()

    set_target_properties(vixen-cli PROPERTIES FOLDER Applications)
else()
    add_precompiled_header(vixen-cli src/pch.hpp PCH_PATH pch.hpp SOURCE_CXX "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp" FORCEINCLUDE)
endif()

##############################
# Installation
#
install(TARGETS vixen-cli
    EXPORT platform-check
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
