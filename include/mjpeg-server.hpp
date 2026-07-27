#pragma once

#include "httplib.h"
#include "feed.hpp"

class MJPEGServer
{
public:
    MJPEGServer(Feed *feed);
    void startServer();

private:
    bool writeToSink(httplib::DataSink &sink, FrameBuffer jpeg);

    Feed *feed;
};