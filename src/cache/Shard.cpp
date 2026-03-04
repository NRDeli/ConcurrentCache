#include "Shard.h"

bool Shard::get(const std::string &key, std::string &value)
{

    std::lock_guard<std::mutex> lock(mtx);

    auto it = map.find(key);

    if (it == map.end())
        return false;

    value = it->second.value;

    return true;
}

void Shard::set(const std::string &key, const std::string &value)
{

    std::lock_guard<std::mutex> lock(mtx);

    map[key] = Entry{value};
}

bool Shard::del(const std::string &key)
{

    std::lock_guard<std::mutex> lock(mtx);

    return map.erase(key) > 0;
}