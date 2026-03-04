#include "ConcurrentCache.h"

ConcurrentCache::ConcurrentCache(size_t shard_count)
    : shards(shard_count) {}

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

    shards[shard_for(key)].set(key, value, ttl_ms);
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