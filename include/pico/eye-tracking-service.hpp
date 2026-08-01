#pragma once

#include <android/binder_ibinder.h>
#include <string>

enum TransactionId
{
    INITIALIZE = FIRST_CALL_TRANSACTION,
    SET_TRACKING_MODE,
    RESET_TRACKING,
    START,
    STOP,
    START_ALGORITHM,
    SET_ALGORITHM_PARAMETERS,
    GET_ALGORITHM_RESULT,
    STOP_ALGORITHM,
    SET_CAMERA_PARAMETERS,
    ADD_SERVICE_LISTENER,
    REMOVE_SERVICE_LISTENER,
    OPEN_CAMERA,
    START_PREVIEW,
    STOP_PREVIEW,
    CLOSE_CAMERA,
    GET_CAMERA_PARAMETERS,
    GET_TRACKING_DATA_SHARED_MEMORY,
    GET_CAMERA_FRAME_SHARED_MEMORY,
    SET_CAMERA_ERROR_LISTENER,
    ADD_TO_SPECIFIC_LIST,
    SET_DATA,
    GET_DATA,
    GET_PUPIL_DISTANCE,
    HAS_EYE_CAMERA,
    REGISTER_IPD_CALLBACK,
    SET_IPD,
    FINISH_IPD_CALIBRATION
};

class EyeTrackingService
{
public:
    // int Initialize(std::string param_1);
    // int SetTrackingMode(int param_1);
    // int ResetTracking(int param_1, int param_2);
    // int Start(std::string param_1);
    // int Stop(std::string param_1);
    // int StartAlgorithm(int param_1, std::string param_2, int param_3);
    // int SetAlgorithmParameters(int type, std::string key, std::string value);
    // int GetAlgorithmResult(int param_1, std::string &param_2);
    // int StopAlgorithm(int param_1, int param_2);
    // int SetCameraParameters(int type, std::string key, std::string value);
    // void AddServiceListener();
    // void RemoveServiceListener();
    // int OpenCamera(int param_1, void *unknown);
    // void StartPreview(int param_1);
    // void StopPreview(int param_1);
    // void CloseCamera(int param_1);
    // void GetCameraParameters(std::string param_1);
    // int GetTrackingDataSharedMemory(int param_1, int* fd);
    static int GetCameraFrameSharedMemory(int camera, int *fd);
    // void SetCameraErrorListener(int param_1);
    // int AddToSpecifiedList(int param_1);
    // int SetData(int param_1, float &param_2);
    // int GetData(int param_1, float &param_2);
    // float GetPupilDistance();
    // bool HasEyeCamera();
    // void RegisterIPDCallback(int param_1);
    // void SetIPD(float param_1);
    // void FinishIPDCalibration(int param_1, bool param_2);
};