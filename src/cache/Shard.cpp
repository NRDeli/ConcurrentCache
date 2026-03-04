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

    map[key] = Entry{value, expire};
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