/**
 * Helper file for native functions from libbinder_ndk.so, because my NDK doesn't have the header files.
 */

#include <sys/prctl.h>
#include <dlfcn.h>

#include "libbinder-ndk.hpp"

using StartThreadPoolFn = void (*)();

void LibBinderNdk::ABinderProcess_startThreadPool()
{
    void *handle = dlopen("libbinder_ndk.so", RTLD_NOW);
    if (!handle)
    {
        return;
    }

    auto startThreadPool = reinterpret_cast<StartThreadPoolFn>(dlsym(handle, "ABinderProcess_startThreadPool"));
    if (startThreadPool)
        startThreadPool();
}