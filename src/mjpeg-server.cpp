#include <thread>
#include <string>

#include "mjpeg-server.hpp"
#include "httplib.h"
#include "log.hpp"

MJPEGServer::MJPEGServer(Feed *feed)
{
    this->feed = feed;
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

    svr.Get("/left", [this](const httplib::Request &req, httplib::Response &res)
            { res.set_content_provider("multipart/x-mixed-replace; boundary=frame", [this](size_t offset, httplib::DataSink &sink) -> bool
                                       {
            static thread_local unsigned long lastFrame = 0;

            FrameBuffer jpeg = feed->waitForLeftEyeBuffer(&lastFrame);
            if (jpeg->empty()) return false;

            return writeToSink(sink, jpeg); }); });

    svr.Get("/right", [this](const httplib::Request &req, httplib::Response &res)
            { res.set_content_provider("multipart/x-mixed-replace; boundary=frame", [this](size_t offset, httplib::DataSink &sink) -> bool
                                       {
            static thread_local unsigned long lastFrame = 0;

            FrameBuffer jpeg = feed->waitForRightEyeBuffer(&lastFrame);
            if (jpeg->empty()) return false;

            return writeToSink(sink, jpeg); }); });

    svr.Get("/face", [this](const httplib::Request &req, httplib::Response &res)
            { res.set_content_provider("multipart/x-mixed-replace; boundary=frame", [this](size_t offset, httplib::DataSink &sink) -> bool
                                       {
            static thread_local unsigned long lastFrame = 0;

            FrameBuffer jpeg = feed->waitForFaceBuffer(&lastFrame);
            if (jpeg->empty()) return false;

            return writeToSink(sink, jpeg); }); });

    svr.listen("0.0.0.0", 9100);
}
