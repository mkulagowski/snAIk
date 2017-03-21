#include "Timer.hpp"

namespace
{
static double GetCounterPeriod()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return 1.0 / static_cast<double>(freq.QuadPart);
}

static const double mPeriod = GetCounterPeriod();
}


Timer::Timer()
{
    mStart.QuadPart = 0;
}

Timer::~Timer()
{
}

void Timer::Start()
{
    QueryPerformanceCounter(&mStart);
}

double Timer::Stop()
{
    LARGE_INTEGER stop;
    QueryPerformanceCounter(&stop);

    return static_cast<double>(stop.QuadPart - mStart.QuadPart) * mPeriod;
}
