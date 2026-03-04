#pragma once

#include "Shard.h"

#include <vector>
#include <functional>
#include <thread>

class ConcurrentCache
{

public:
    explicit ConcurrentCache(size_t shard_count);
    ~ConcurrentCache();

    bool get(const std::string &key, std::string &value);

    void set(const std::string &, const std::string &, uint64_t ttl_ms);

    bool del(const std::string &key);

    bool expire(const std::string &, uint64_t ttl_ms);

    int64_t ttl(const std::string &);

    size_t shard_for(const std::string &key);

    void expirer_loop();

private:
    std::vector<Shard> shards;

    size_t max_bytes = 256 * 1024 * 1024;

    size_t total_bytes = 0;

    std::thread expirer_thread;

    bool running = true;
};