cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_STANDARD 17)

project(PicoIPCameraDaemon CXX)

include(ExternalProject)
include(embed.cmake)

#JPEG_TURBO
set(JPEG_TURBO_SOURCE     "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libjpeg-turbo")

set(JPEG_TURBO_PREFIX     "${CMAKE_BINARY_DIR}/third_party/libjpeg-turbo")
set(JPEG_TURBO_INSTALL_DIR "${JPEG_TURBO_PREFIX}/install")

ExternalProject_Add(libjpeg_turbo
    SOURCE_DIR        "${JPEG_TURBO_SOURCE}"
    PREFIX            "${JPEG_TURBO_PREFIX}"
    BINARY_DIR        "${JPEG_TURBO_PREFIX}/src/libjpeg_turbo_project"

    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DANDROID_TOOLCHAIN=${TOOLCHAIN}
        -DANDROID_ABI=${ANDROID_ABI}
        -DANDROID_PLATFORM=${ANDROID_PLATFORM}
        -DANDROID_NDK=${ANDROID_NDK}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_ASM_FLAGS="--target=aarch64-linux-android${ANDROID_VERSION}"
        -DENABLE_SHARED=OFF
        -DENABLE_STATIC=ON
        -DWITH_SIMD=ON
        -DWITH_SYSTEM_ZLIB=ON
        -DWITH_TURBOJPEG=ON
        -DWITH_ARITH_ENC=OFF
        -DWITH_ARITH_DEC=OFF
        -DWITH_TURBOJPEG=ON
        -DWITH_JPEG8=OFF

    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target turbojpeg-static --config ${CMAKE_BUILD_TYPE}

    INSTALL_COMMAND
        COMMAND ${CMAKE_COMMAND} -E copy
            <BINARY_DIR>/libturbojpeg.a
            ${JPEG_TURBO_INSTALL_DIR}/lib/libturbojpeg.a
)

set(JPEG_TURBO_LIBRARY "${JPEG_TURBO_INSTALL_DIR}/lib/libturbojpeg.a")

add_library(libturbojpeg STATIC IMPORTED GLOBAL)
set_target_properties(libturbojpeg PROPERTIES
    IMPORTED_LOCATION ${JPEG_TURBO_LIBRARY}
)

add_dependencies(libturbojpeg libjpeg_turbo)

#HTTPLIB
set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE OFF)
set(HTTPLIB_USE_ZLIB_IF_AVAILABLE OFF)
set(HTTPLIB_USE_BROTLI_IF_AVAILABLE OFF)
set(HTTPLIB_USE_ZSTD_IF_AVAILABLE OFF)

add_subdirectory(third_party/cpp-httplib)

#nlohmann/json
add_subdirectory(third_party/json)

set(SOURCES
    src/main.cpp
    src/leds.cpp
    src/camera.cpp
    src/libbinder-ndk.cpp
    src/mjpeg-server.cpp
    src/feed.cpp
    src/camera-shared-memory.cpp
    src/pico/eye-tracking-service.cpp
)

add_executable(picoipcamerad ${SOURCES})

target_embed_files(picoipcamerad FILES ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html)

target_compile_options(picoipcamerad PUBLIC -Og -ffunction-sections -fdata-sections)
target_link_options(picoipcamerad PUBLIC 
)

target_include_directories(picoipcamerad PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    ${JPEG_TURBO_SOURCE}/src
    third_party/cpp-httplib
    third_party/json/single_include/
)
target_link_libraries(picoipcamerad 
    camera2ndk
    mediandk 
    -landroid 
    binder_ndk
    log
    libturbojpeg
    httplib::httplib
)