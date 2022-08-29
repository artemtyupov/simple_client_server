cmake_minimum_required(VERSION 3.20)

# prevent in-source build
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR “In-source build detected!”)
endif()

# register custom cmake modules folder
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(TcpClientServerApp)

if(NOT ${CMAKE_SIZEOF_VOID_P} EQUAL 4)
    set(_PLATFORM_X64 TRUE)
    set(_PLATFORM_DIR x64)
else()
    set(_PLATFORM_DIR x32)
endif()

if(WIN32)
    macro(get_WIN32_WINNT version)
        if(CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif()
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif()
    endmacro()

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${_PLATFORM_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${_PLATFORM_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${_PLATFORM_DIR}/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${_PLATFORM_DIR}/pdb)
set(CMAKE_TESTS_OUTPUT_DIR ${PROJECT_BINARY_DIR}/${_PLATFORM_DIR}/tests)

include(include_cmn_libs)

list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(server)
add_subdirectory(client)
