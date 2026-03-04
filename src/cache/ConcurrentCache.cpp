#include "ConcurrentCache.h"

ConcurrentCache::ConcurrentCache(size_t shard_count)
    : shards(shard_count)
{
    running = true;
    expirer_thread = std::thread(&ConcurrentCache::expirer_loop, this);
}

size_t ConcurrentCache::shard_for(const std::string &key)
{

    std::hash<std::string> h;

    return h(key) % shards.size();
}

bool ConcurrentCache::get(const std::string &key, std::string &value)
{

    return shards[shard_for(key)].get(key, value);
}

void ConcurrentCache::set(const std::string &key,
                          const std::string &value,
                          uint64_t ttl_ms)
{

    size_t shard_id = shard_for(key);

    shards[shard_id].set(key, value, ttl_ms);

    total_bytes += key.size() + value.size();

    if (total_bytes > max_bytes)
    {

        size_t over = total_bytes - max_bytes;

        for (auto &shard : shards)
        {
            size_t freed = shard.evict_if_needed(over);
            if (freed > total_bytes)
                total_bytes = 0;
            else
                total_bytes -= freed;

            if (over == 0)
                break;
        }
    }
}

bool ConcurrentCache::del(const std::string &key)
{

    return shards[shard_for(key)].del(key);
}

bool ConcurrentCache::expire(const std::string &key,
                             uint64_t ttl_ms)
{

    return shards[shard_for(key)].expire(key, ttl_ms);
}

int64_t ConcurrentCache::ttl(const std::string &key)
{

    return shards[shard_for(key)].ttl(key);
}

void ConcurrentCache::expirer_loop()
{
    while (running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        for (auto &shard : shards)
            shard.scan_expired(20);
    }
}

ConcurrentCache::~ConcurrentCache()
{
    running = false;

    if (expirer_thread.joinable())
        expirer_thread.join();
}