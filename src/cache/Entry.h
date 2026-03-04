#pragma once

#include <string>
#include <cstdint>

struct Entry
{
    std::string value;
    uint64_t expire_at_ms = 0;
};