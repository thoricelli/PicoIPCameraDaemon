#include <android/binder_parcel.h>
#include <android/binder_status.h>
#include <dlfcn.h>

#include "pico/eye-tracking-service.hpp"

#define SERVICE "pxreyetrackingservice"

using AServiceManager_getService = AIBinder *(*)(const char *service);

// TODO move this
AIBinder *ndkGetService(const char *service)
{
    void *handle = dlopen("libbinder_ndk.so", RTLD_NOW);
    if (!handle)
        return nullptr;

    AServiceManager_getService getService = reinterpret_cast<AServiceManager_getService>(dlsym(handle, "AServiceManager_getService"));
    if (!getService)
        return nullptr;

    AIBinder *binder = getService(service);

    dlclose(handle);

    return binder;
}

static void *onCreate(void *args) { return args; }
static void onDestroy(void *userData) {}
static binder_status_t onTransact(AIBinder *, transaction_code_t, const AParcel *, AParcel *)
{
    return STATUS_UNKNOWN_TRANSACTION;
}

int EyeTrackingService::GetCameraFrameSharedMemory(int camera, int *fd)
{
    // TODO: Cache this!
    AIBinder *binder = ndkGetService(SERVICE);

    if (!binder)
        return -1;

    AParcel *inParcel = nullptr;
    AParcel *outParcel = nullptr;
    binder_status_t status = EX_TRANSACTION_FAILED;

    AIBinder_Class *clazz = AIBinder_Class_define("pvr.IEyeTrackingService", onCreate, onDestroy, onTransact);
    AIBinder_associateClass(binder, clazz);

    binder_status_t binderStatus = AIBinder_prepareTransaction(binder, &inParcel);

    if (binderStatus == STATUS_OK)
    {
        AParcel_writeInt32(inParcel, camera);

        status = AIBinder_transact(
            binder,
            GET_CAMERA_FRAME_SHARED_MEMORY,
            &inParcel,
            &outParcel,
            0);

        if (status == STATUS_OK)
            AParcel_readParcelFileDescriptor(outParcel, fd);

        if (inParcel)
            AParcel_delete(inParcel);
        if (outParcel)
            AParcel_delete(outParcel);
    }

    AIBinder_decStrong(binder);

    return status;
}