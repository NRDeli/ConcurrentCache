#pragma once

#include "Shard.h"

#include <vector>
#include <functional>

class ConcurrentCache
{

public:
    explicit ConcurrentCache(size_t shard_count);

    bool get(const std::string &key, std::string &value);

    void set(const std::string &, const std::string &, uint64_t ttl_ms);

    bool del(const std::string &key);

    bool expire(const std::string &, uint64_t ttl_ms);

    int64_t ttl(const std::string &);

private:
    std::vector<Shard> shards;

    size_t shard_for(const std::string &key);
};