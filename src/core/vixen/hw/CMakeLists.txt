file(GLOB DIR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ata/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ata/cmds/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ata/drvs/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/atapi/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/atapi/cmds/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basic/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/bus/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sm/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pci/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pci/bmide/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nv2a/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ohci/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/xid/*.h
    )

file(GLOB DIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ata/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ata/cmds/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ata/drvs/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/atapi/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/atapi/cmds/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basic/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bus/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sm/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pci/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pci/bmide/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ohci/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/xid/*.cpp
    )

if (WIN32)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/basic/win32")
endif (WIN32)


set(SOURCES ${SOURCES}
    ${DIR_HEADERS}
    ${DIR_SOURCES}
    PARENT_SCOPE
    )
