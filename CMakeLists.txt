cmake_minimum_required (VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
project (openxbox)

option(USE_CPU_UNICORN "Use Unicorn CPU emulator" ON)

# Add source directories
set(SOURCES, "")
add_subdirectory("${CMAKE_SOURCE_DIR}/src")

# Main Executable
add_definitions("-Wall -Werror -O0 -g")
add_executable(openxbox ${SOURCES})
target_include_directories(openxbox PUBLIC ${CMAKE_SOURCE_DIR}/src)

# SDL2
find_package(SDL2 REQUIRED)
target_link_libraries(openxbox ${SDL2_LIBRARY})
target_include_directories(openxbox PUBLIC ${SDL2_INCLUDE_DIR})

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(openxbox ${OPENGL_gl_LIBRARY})
target_include_directories(openxbox PUBLIC ${OPENGL_INCLUDE_DIR})

# GL Extension Wrangler (GLEW)
find_package(GLEW REQUIRED)
target_link_libraries(openxbox ${GLEW_LIBRARIES})
target_include_directories(openxbox PUBLIC ${GLEW_INCLUDE_DIRS})

# Unicorn CPU Emulator
if (USE_CPU_UNICORN)
find_package(Unicorn REQUIRED)
add_definitions("-DCPU_UNICORN")
target_link_libraries(openxbox ${LIBUNICORN_LIBRARY})
target_include_directories(openxbox PUBLIC ${LIBUNICORN_INCLUDE_DIR})
endif (USE_CPU_UNICORN)
