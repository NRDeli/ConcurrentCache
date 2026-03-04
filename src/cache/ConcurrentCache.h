#pragma once

#include "Shard.h"

#include <vector>
#include <functional>

class ConcurrentCache
{

public:
    explicit ConcurrentCache(size_t shard_count);

    bool get(const std::string &key, std::string &value);

    void set(const std::string &key, const std::string &value);

    bool del(const std::string &key);

private:
    std::vector<Shard> shards;

    size_t shard_for(const std::string &key);
};