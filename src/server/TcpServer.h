#pragma once

#include "Worker.h"

#include <vector>
#include <memory>

class TcpServer
{
public:
    TcpServer(int port, int workers);

    void run();

private:
    int port;
    int listen_fd;

    std::vector<std::unique_ptr<Worker>> worker_pool;
};