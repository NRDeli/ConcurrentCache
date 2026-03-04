#include "Worker.h"
#include "../util/Logger.h"

#include <unistd.h>
#include <vector>

Worker::Worker() : running(true) {}

void Worker::start()
{
    worker_thread = std::thread(&Worker::loop, this);
}

void Worker::enqueue(int fd)
{

    connections.emplace(fd, Connection(fd));
    reactor.add_fd(fd, false);
}

void Worker::loop()
{

    Logger::info("Worker thread started");

    while (running)
    {

        std::vector<Event> events;

        reactor.wait(events, 1000);

        for (auto &ev : events)
        {

            auto it = connections.find(ev.fd);

            if (it == connections.end())
            {
                continue;
            }

            auto &conn = it->second;

            if (ev.readable)
            {

                char buffer[1024];

                int n = read(ev.fd, buffer, sizeof(buffer));

                if (n <= 0)
                {
                    close(ev.fd);
                    reactor.del_fd(ev.fd);
                    connections.erase(ev.fd);
                    continue;
                }

                conn.outbuf.append(buffer, n);

                write(ev.fd, conn.outbuf.data(), conn.outbuf.size());

                conn.outbuf.clear();
            }
        }
    }
}