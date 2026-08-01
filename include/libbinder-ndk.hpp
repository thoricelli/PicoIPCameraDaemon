#pragma once

using StartThreadPool = void (*)();

class LibBinderNdk
{
public:
    static void ABinderProcess_startThreadPool();

private:
    static StartThreadPool aBinderProcess_startThreadPool;
};