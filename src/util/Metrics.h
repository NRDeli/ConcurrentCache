#pragma once

#include <atomic>

struct Metrics
{

    std::atomic<uint64_t> requests{0};

    std::atomic<uint64_t> get_hits{0};

    std::atomic<uint64_t> get_misses{0};

    std::atomic<uint64_t> evictions{0};
};

extern Metrics metrics;