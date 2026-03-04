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

void ConcurrentCache::set(const std::string &key, const std::string &value)
{

    shards[shard_for(key)].set(key, value);
}

bool ConcurrentCache::del(const std::string &key)
{

    return shards[shard_for(key)].del(key);
}