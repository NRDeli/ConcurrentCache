#pragma once

#include "Reactor.h"
#include "Connection.h"
#include "Parser.h"
#include "../cache/ConcurrentCache.h"

#include <unordered_map>
#include <thread>
#include <atomic>

class Worker
{
public:
    Worker();

    void start();
    void enqueue(int fd);

private:
    void loop();

    Reactor reactor;
    std::unordered_map<int, Connection> connections;

    std::thread worker_thread;
    std::atomic<bool> running;

    ConcurrentCache cache{64};
};