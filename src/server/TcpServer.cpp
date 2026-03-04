#include "TcpServer.h"
#include "../util/Logger.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

TcpServer::TcpServer(int port_, int workers) : port(port_)
{

    for (int i = 0; i < workers; i++)
    {
        worker_pool.push_back(std::make_unique<Worker>());
        worker_pool.back()->start();
    }
}

void TcpServer::run()
{

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_fd, (sockaddr *)&addr, sizeof(addr));

    listen(listen_fd, 128);

    Logger::info("Server listening...");

    int worker_index = 0;

    while (true)
    {

        int client_fd = accept(listen_fd, nullptr, nullptr);

        worker_pool[worker_index]->enqueue(client_fd);

        worker_index = (worker_index + 1) % worker_pool.size();
    }
}