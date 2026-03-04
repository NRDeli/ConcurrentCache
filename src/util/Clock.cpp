#include "Clock.h"
#include <chrono>

uint64_t Clock::now_ms()
{

    using namespace std::chrono;

    return duration_cast<milliseconds>(
               steady_clock::now().time_since_epoch())
        .count();
}