#include "Shard.h"
#include "../util/Clock.h"

bool Shard::get(const std::string &key, std::string &value)
{

    std::lock_guard<std::mutex> lock(mtx);

    auto it = map.find(key);

    if (it == map.end())
        return false;

    if (it->second.expire_at_ms &&
        Clock::now_ms() > it->second.expire_at_ms)
    {

        map.erase(it);
        return false;
    }

    value = it->second.value;

    lru.erase(it->second.lru_it);
    lru.push_front(key);
    it->second.lru_it = lru.begin();

    return true;
}

void Shard::set(const std::string &key,
                const std::string &value,
                uint64_t ttl_ms)
{

    std::lock_guard<std::mutex> lock(mtx);

    uint64_t expire = 0;

    if (ttl_ms > 0)
        expire = Clock::now_ms() + ttl_ms;

    size_t bytes = key.size() + value.size();

    auto it = map.find(key);

    if (it != map.end())
    {

        bytes_used -= it->second.bytes;

        lru.erase(it->second.lru_it);
    }

    lru.push_front(key);

    Entry e;

    e.value = value;
    e.expire_at_ms = expire;
    e.lru_it = lru.begin();
    e.bytes = bytes;

    map[key] = e;

    bytes_used += bytes;
}

bool Shard::del(const std::string &key)
{

    std::lock_guard<std::mutex> lock(mtx);

    return map.erase(key) > 0;
}

bool Shard::expire(const std::string &key, uint64_t ttl_ms)
{

    std::lock_guard<std::mutex> lock(mtx);

    auto it = map.find(key);

    if (it == map.end())
        return false;

    it->second.expire_at_ms = Clock::now_ms() + ttl_ms;

    return true;
}

int64_t Shard::ttl(const std::string &key)
{

    std::lock_guard<std::mutex> lock(mtx);

    auto it = map.find(key);

    if (it == map.end())
        return -2;

    if (it->second.expire_at_ms == 0)
        return -1;

    int64_t remaining =
        it->second.expire_at_ms - Clock::now_ms();

    if (remaining <= 0)
    {
        map.erase(it);
        return -2;
    }

    return remaining / 1000;
}

size_t Shard::evict_if_needed(size_t &bytes_over)
{
    std::lock_guard<std::mutex> lock(mtx);

    size_t freed = 0;

    while (bytes_over > 0 && !lru.empty())
    {
        std::string victim = lru.back();
        lru.pop_back();

        auto it = map.find(victim);
        if (it == map.end())
            continue;

        const size_t victim_bytes = it->second.bytes;

        // update accounting
        bytes_used -= victim_bytes;
        freed += victim_bytes;

        // reduce the remaining "over" budget
        if (victim_bytes >= bytes_over)
            bytes_over = 0;
        else
            bytes_over -= victim_bytes;

        map.erase(it);
    }

    return freed; // BYTES freed
}

size_t Shard::scan_expired(size_t limit)
{
    std::lock_guard<std::mutex> lock(mtx);

    size_t removed = 0;

    auto now = Clock::now_ms();

    for (auto it = map.begin(); it != map.end() && removed < limit;)
    {
        if (it->second.expire_at_ms &&
            now > it->second.expire_at_ms)
        {
            lru.erase(it->second.lru_it);

            bytes_used -= it->second.bytes;

            it = map.erase(it);

            removed++;
        }
        else
        {
            ++it;
        }
    }

    return removed;
}