#pragma once

#include <Windows.h>


class Timer
{

public:
    Timer();
    ~Timer();

    void Start();
    double Stop();

private:
    LARGE_INTEGER mStart;
};
