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

    BUILD_BYPRODUCTS  "${JPEG_TURBO_INSTALL_DIR}/lib/libturbojpeg.a"

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
    src/mjpeg-server.cpp
    src/feed.cpp
    src/camera-shared-memory.cpp
    src/pvr/pxr-eye-tracking-service.cpp
    src/data-buffer.cpp
)

#PICO Android OS libs
add_library(binder SHARED IMPORTED)
set_target_properties(binder PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libbinder.so
)

add_library(utils SHARED IMPORTED)
set_target_properties(utils PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libutils.so
)

add_library(cutils SHARED IMPORTED)
set_target_properties(cutils PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libcutils.so
)

add_library(base SHARED IMPORTED)
set_target_properties(base PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libbase.so
)

add_library(binder_ndk_real SHARED IMPORTED)
set_target_properties(binder_ndk_real PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libbinder_ndk.so
)


add_executable(picoipcamerad ${SOURCES})

target_embed_files(picoipcamerad FILES ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html ${CMAKE_CURRENT_SOURCE_DIR}/html/style.css ${CMAKE_CURRENT_SOURCE_DIR}/html/dist/script.js)

target_compile_options(picoipcamerad PRIVATE 
    -fno-rtti
    -fno-exceptions

    # Debug Configuration (No optimization, maximum debuggability)
    $<$<CONFIG:Debug>:
        -O0
        -g3
        -fno-omit-frame-pointer
    >

    # Release Configuration (Maximum optimization)
    $<$<CONFIG:Release>:
        -O3
        -flto
        -ffunction-sections
        -fdata-sections
        -fvisibility=hidden
    >
)

target_compile_definitions(picoipcamerad PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

target_link_options(picoipcamerad PRIVATE
    -Wl,--allow-shlib-undefined

    $<$<CONFIG:Release>:
        -flto
        -Wl,--gc-sections
        -Wl,-s
    >
)

target_include_directories(picoipcamerad PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    ${JPEG_TURBO_SOURCE}/src
    third_party/cpp-httplib
    third_party/json/single_include/
)
target_include_directories(picoipcamerad PRIVATE
    third_party/native/libs/binder/include
    third_party/native/libs/binder/ndk/include_apex
    third_party/core/libutils/include
    third_party/core/liblog/include
    third_party/core/base/include
    third_party/core/libcutils/include
    third_party/core/libsystem/include
)
target_link_libraries(picoipcamerad 
    camera2ndk
    mediandk 
    -landroid 
    binder
    utils
    cutils
    base
    binder_ndk_real
    log
    libturbojpeg
    httplib::httplib
)