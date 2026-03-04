#pragma once

#include "Entry.h"

#include <unordered_map>
#include <mutex>
#include <list>

class Shard
{

public:
    bool get(const std::string &key, std::string &value);

    void set(const std::string &key,
             const std::string &value,
             uint64_t ttl_ms);

    bool del(const std::string &key);

    bool expire(const std::string &key, uint64_t ttl_ms);

    int64_t ttl(const std::string &key);

    size_t scan_expired(size_t limit);

    size_t evict_if_needed(size_t &bytes_over); // returns BYTES freed

private:
    std::unordered_map<std::string, Entry> map;

    std::list<std::string> lru;

    std::mutex mtx;

    size_t bytes_used = 0;
};