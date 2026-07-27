cmake_minimum_required(VERSION 3.22.1)
project(PicoIPCameraDaemon CXX)

set(SOURCES
    src/main.cpp
    src/led.cpp
    src/camera.cpp
    src/libbinder_ndk.cpp

    include/camera.hpp
    include/led.hpp
    include/libbinder_ndk.hpp
    include/log.hpp
)
add_executable(picoipcamerad ${SOURCES})

target_compile_options(picoipcamerad PUBLIC -g -Wall -Wshadow -O0)

find_library(log-lib log)

target_include_directories(picoipcamerad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(picoipcamerad camera2ndk mediandk -landroid ${log-lib})