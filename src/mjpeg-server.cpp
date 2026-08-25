#include <thread>
#include <string>
#include "httplib.h"

// Assets
#include <embed/picoipcamerad.h>

#include "mjpeg-server.hpp"
#include "log.hpp"
#include "leds.hpp"

MJPEGServer::MJPEGServer(ICamera *camera, Feed *feed)
{
    this->feed = feed;
    this->camera = camera;
}

bool MJPEGServer::writeToSink(httplib::DataSink &sink, FrameBuffer jpeg)
{
    std::ostringstream header;
    header << "--frame\r\n"
           << "Content-Type: image/jpeg\r\n"
           << "Content-Length: " << jpeg->size() << "\r\n\r\n";

    std::string h = header.str();
    if (!sink.write(h.data(), h.size()))
        return false;
    if (!sink.write(reinterpret_cast<const char *>(jpeg->data()), jpeg->size()))
        return false;
    if (!sink.write("\r\n", 2))
        return false;

    return true;
}

void MJPEGServer::startServerBlocking()
{
    httplib::Server svr;

    this->timerThread = std::thread([this]()
                                    { this->closeTimerLoop(); });

    svr.Get("/", [this](const httplib::Request &req, httplib::Response &res)
            { res.set_content(reinterpret_cast<const char *>(index), sizeof(index), "text/html"); });

    svr.Get("/style.css", [this](const httplib::Request &req, httplib::Response &res)
            { res.set_content(reinterpret_cast<const char *>(style), sizeof(style), "text/css"); });

    svr.Get("/script.js", [this](const httplib::Request &req, httplib::Response &res)
            { res.set_content(reinterpret_cast<const char *>(script), sizeof(script), "text/javascript"); });

    svr.Get("/left", [this](const httplib::Request &req, httplib::Response &res)
            { this->leftEyeEndpoint(req, res); });

    svr.Get("/right", [this](const httplib::Request &req, httplib::Response &res)
            { this->rightEyeEndpoint(req, res); });

    svr.Get("/face", [this](const httplib::Request &req, httplib::Response &res)
            { this->faceEndpoint(req, res); });

    svr.listen("0.0.0.0", 9100);
}

void MJPEGServer::leftEyeEndpoint(const httplib::Request &req, httplib::Response &res)
{
    this->ensureCameraOpen();
    this->feed->setLeftEyeBufferState(true);
    this->leftEyeListeners++;

    res.set_content_provider("multipart/x-mixed-replace; boundary=frame", [this](size_t offset, httplib::DataSink &sink) -> bool
                             {
        static thread_local unsigned long lastFrame = 0;

        FrameBuffer jpeg = this->feed->waitForLeftEyeBuffer(&lastFrame);
        if (jpeg->empty()) return false;

        return writeToSink(sink, jpeg); }, [this](bool success)
                             { 
        this->leftEyeListeners--; 

        if (leftEyeListeners <= 0)
            this->feed->setLeftEyeBufferState(false);

        this->checkCameraShouldStayOpen(); });
}

void MJPEGServer::rightEyeEndpoint(const httplib::Request &req, httplib::Response &res)
{
    this->ensureCameraOpen();
    this->feed->setRightEyeBufferState(true);
    this->rightEyeListeners++;

    res.set_content_provider("multipart/x-mixed-replace; boundary=frame", [this](size_t offset, httplib::DataSink &sink) -> bool
                             {
    static thread_local unsigned long lastFrame = 0;

    FrameBuffer jpeg = this->feed->waitForRightEyeBuffer(&lastFrame);
    if (jpeg->empty()) return false;

    return writeToSink(sink, jpeg); }, [this](bool success)
                             { 
    this->rightEyeListeners--; 

    if (rightEyeListeners <= 0)
        this->feed->setRightEyeBufferState(false);

    this->checkCameraShouldStayOpen(); });
}

void MJPEGServer::faceEndpoint(const httplib::Request &req, httplib::Response &res)
{

    this->ensureCameraOpen();
    this->feed->setFaceBufferState(true);
    this->faceListeners++;

    res.set_content_provider("multipart/x-mixed-replace; boundary=frame", [this](size_t offset, httplib::DataSink &sink) -> bool
                             {
                    static thread_local unsigned long lastFrame = 0;

                    FrameBuffer jpeg = this->feed->waitForFaceBuffer(&lastFrame);
                    if (jpeg->empty()) return false;

                    return writeToSink(sink, jpeg); }, [this](bool success)
                             { 
                    this->faceListeners--;

                    if (faceListeners <= 0)
                        this->feed->setFaceBufferState(false);

                    this->checkCameraShouldStayOpen(); });
}

void MJPEGServer::ensureCameraOpen()
{
    if (this->isOpening.exchange(true))
    {
        return;
    }

    this->cancelCameraCloseTimer();

    if (this->camera->isOpen())
        return;

    this->camera->openCamera();

    this->isOpening.store(false);
}

void MJPEGServer::checkCameraShouldStayOpen()
{
    if (this->faceListeners > 0 && this->leftEyeListeners > 0 && this->rightEyeListeners > 0)
        return;

    this->startCameraCloseTimer();
}

void MJPEGServer::closeTimerLoop()
{
    while (true)
    {
        if (this->timerActive)
        {
            if (std::chrono::steady_clock::now() >= this->triggerStart)
            {
                this->timerActive = false;
                this->isOpening.store(false);
                this->camera->closeCamera();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MJPEGServer::startCameraCloseTimer()
{
    this->triggerStart = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    this->timerActive = true;
}

void MJPEGServer::cancelCameraCloseTimer()
{
    this->timerActive = false;
}