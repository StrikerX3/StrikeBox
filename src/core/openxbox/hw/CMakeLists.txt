file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basic/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/bus/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sm/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pci/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nv2a/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basic/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bus/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sm/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pci/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nv2a/*.cpp
    )

if (WIN32)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/basic/win32")
endif (WIN32)


set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    PARENT_SCOPE
    )
