/**
 * Helper file for native functions from libbinder_ndk.so, because my NDK doesn't have the header files.
 */

#include <sys/prctl.h>
#include <dlfcn.h>

#include "libbinder-ndk.hpp"

#define LIBRARY "libbinder_ndk.so"

StartThreadPool LibBinderNdk::aBinderProcess_startThreadPool = nullptr;

void LibBinderNdk::ABinderProcess_startThreadPool()
{
    if (LibBinderNdk::aBinderProcess_startThreadPool)
        LibBinderNdk::aBinderProcess_startThreadPool();

    void *handle = dlopen(LIBRARY, RTLD_NOW);
    if (!handle)
    {
        return;
    }

    StartThreadPool startThreadPool = reinterpret_cast<StartThreadPool>(dlsym(handle, "ABinderProcess_startThreadPool"));
    if (startThreadPool)
    {
        LibBinderNdk::aBinderProcess_startThreadPool = startThreadPool;
        startThreadPool();
    }

    dlclose(handle);
}