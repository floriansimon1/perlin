cmake_minimum_required(VERSION 3.23)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")

    file(DOWNLOAD   "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                    "${CMAKE_BINARY_DIR}/conan.cmake"
         TLS_VERIFY ON)
endif()

project(perlin CXX)

set(CMAKE_CXX_STANDARD          14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(${CMAKE_BINARY_DIR}/conan.cmake)

set(CMAKE_BUILD_TYPE Debug)

conan_cmake_configure(REQUIRES   sfml/2.5.1
                      GENERATORS cmake_find_package_multi
                      OPTIONS    "sfml:shared=True")

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD             missing
                    REMOTE            conancenter
                    SETTINGS          ${settings})

add_subdirectory(source)
