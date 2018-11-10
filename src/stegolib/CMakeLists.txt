cmake_minimum_required(VERSION 3.3)
project(stegolib)
include(ExternalProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

file(GLOB_RECURSE SOURCE_FILES
    "*.h"
    "*.cpp"
)

link_directories(${EP_INSTALL}/lib)
include_directories(${EP_INSTALL}/include)
add_library(stegolib SHARED ${SOURCE_FILES})

install(TARGETS stegolib
        LIBRARY DESTINATION lib)
install(FILES stego_connector.h DESTINATION include/stego)

set(RSCODE_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../libs/rscode)
ExternalProject_Add(
        rscode_proj
        BINARY_DIR ${RSCODE_DIR}
        SOURCE_DIR ${RSCODE_DIR}
        INSTALL_DIR ${EP_INSTALL}
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

add_library(librscode STATIC IMPORTED)
set_property(TARGET librscode PROPERTY IMPORTED_LOCATION ${EP_INSTALL}/lib/librscode.a)
add_dependencies(librscode rscode_proj)
add_dependencies(stegolib librscode)
target_link_libraries(stegolib librscode)

set(CRYPTOPP_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../libs/cryptopp)
ExternalProject_Add(
       cryptopp_proj
       BINARY_DIR ${CRYPTOPP_DIR}
       SOURCE_DIR ${CRYPTOPP_DIR}
       CONFIGURE_COMMAND ""
       BUILD_COMMAND make -j$(nproc) static shared
       INSTALL_COMMAND ""
)

add_library(libcryptopp STATIC IMPORTED)
ExternalProject_Get_Property(cryptopp_proj BINARY_DIR)
set_property(TARGET libcryptopp PROPERTY IMPORTED_LOCATION ${BINARY_DIR}/libcryptopp.a)
add_dependencies(libcryptopp cryptopp_proj)
add_dependencies(stegolib libcryptopp)

include_directories(stegolib ${BINARY_DIR})

target_link_libraries(stegolib libcryptopp)