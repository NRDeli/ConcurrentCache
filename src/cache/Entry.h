#pragma once

#include <string>
#include <list>
#include <cstdint>

struct Entry
{

    std::string value;
    uint64_t expire_at_ms = 0;
    std::list<std::string>::iterator lru_it;
    size_t bytes = 0;
};