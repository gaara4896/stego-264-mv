cmake_minimum_required(VERSION 3.3)
project(stego)
include(ExternalProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

add_executable(stego_enc src/encoder.c)
add_executable(stego_dec src/decoder.c)

set(EP_INSTALL ${CMAKE_BINARY_DIR}/install CACHE INTERNAL "")

set(STEGOLIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/stegolib)
set(STEGOLIBSRC_DIR ${CMAKE_SOURCE_DIR}/src/stegolib)
add_subdirectory(src/stegolib)

set(FFMPEG_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs/ffmpeg)
ExternalProject_Add(ffmpeg_proj DEPENDS stegolib
        BINARY_DIR ${FFMPEG_DIR}
        SOURCE_DIR ${FFMPEG_DIR}
        CONFIGURE_COMMAND
            # Install step is very unreliable otherwise
            mkdir -p ${FFMPEG_DIR}/include/stego &&
            mkdir -p ${FFMPEG_DIR}/lib &&
            cp ${STEGOLIBSRC_DIR}/stego_connector.h ${FFMPEG_DIR}/include/stego &&
            cp ${STEGOLIB_DIR}/libstegolib.so ${FFMPEG_DIR}/lib &&
            env LD_PRELOAD=${STEGOLIB_DIR}/libstegolib.so <SOURCE_DIR>/configure
            --prefix=<BINARY_DIR>
            --extra-cflags=-I<BINARY_DIR>/include
            --extra-ldflags=-L<BINARY_DIR>/lib
            --extra-libs=-lstegolib
            --enable-gpl
            --enable-nonfree
            --enable-libfdk-aac
            --disable-doc
            # --disable-pthreads
            --disable-programs
            --enable-shared
            --enable-static
            --enable-runtime-cpudetect
        BUILD_COMMAND make -j$(nproc)
        INSTALL_COMMAND make install
)

add_library(libavcodec SHARED IMPORTED)
add_library(libavformat SHARED IMPORTED)
add_library(libavfilter SHARED IMPORTED)
add_library(libavutil SHARED IMPORTED)
add_library(libstegolib SHARED IMPORTED)

ExternalProject_Get_Property(ffmpeg_proj BINARY_DIR)
set_property(TARGET libavcodec PROPERTY IMPORTED_LOCATION ${BINARY_DIR}/lib/libavcodec.so)
set_property(TARGET libavformat PROPERTY IMPORTED_LOCATION ${BINARY_DIR}/lib/libavformat.so)
set_property(TARGET libavfilter PROPERTY IMPORTED_LOCATION ${BINARY_DIR}/lib/libavfilter.so)
set_property(TARGET libavutil PROPERTY IMPORTED_LOCATION ${BINARY_DIR}/lib/libavutil.so)
set_property(TARGET libstegolib PROPERTY IMPORTED_LOCATION ${STEGOLIB_DIR}/libstegolib.so)

add_dependencies(libavcodec ffmpeg_proj)
add_dependencies(libavformat ffmpeg_proj)
add_dependencies(libavfilter ffmpeg_proj)
add_dependencies(libavutil ffmpeg_proj)

add_dependencies(stego_enc libavcodec)
add_dependencies(stego_enc libavformat)
add_dependencies(stego_enc libavfilter)
add_dependencies(stego_enc libavutil)
add_dependencies(stego_enc libstegolib)

add_dependencies(stego_dec libavcodec)
add_dependencies(stego_dec libavformat)
add_dependencies(stego_dec libavfilter)
add_dependencies(stego_dec libavutil)
add_dependencies(stego_dec libstegolib)

ExternalProject_Get_Property(ffmpeg_proj SOURCE_DIR)
include_directories(stego_enc ${SOURCE_DIR})
include_directories(stego_dec ${SOURCE_DIR})

target_link_libraries(stego_enc libavcodec)
target_link_libraries(stego_enc libavformat)
target_link_libraries(stego_enc libavfilter)
target_link_libraries(stego_enc libavutil)
target_link_libraries(stego_enc libstegolib)

target_link_libraries(stego_dec libavcodec)
target_link_libraries(stego_dec libavformat)
target_link_libraries(stego_dec libavfilter)
target_link_libraries(stego_dec libavutil)
target_link_libraries(stego_dec libstegolib)