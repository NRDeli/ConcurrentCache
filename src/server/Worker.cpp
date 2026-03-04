#include "Worker.h"
#include "Parser.h"
#include "../util/Logger.h"
#include "../util/Metrics.h"

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

                conn.inbuf.append(buffer, n);

                size_t pos;

                while ((pos = conn.inbuf.find('\n')) != std::string::npos)
                {

                    std::string line = conn.inbuf.substr(0, pos);

                    if (!line.empty() && line.back() == '\r')
                        line.pop_back();

                    conn.inbuf.erase(0, pos + 1);

                    auto cmd = Parser::parse(line);
                    metrics.requests++;
                    switch (cmd.type)
                    {

                    case CmdType::PING:
                        conn.outbuf += "+PONG\n";
                        break;

                    case CmdType::SET:
                        cache.set(cmd.key, cmd.value, 0);
                        conn.outbuf += "+OK\n";
                        break;

                    case CmdType::GET:
                    {
                        std::string value;

                        if (!cache.get(cmd.key, value))
                        {
                            metrics.get_misses++;
                            conn.outbuf += "$-1\n";
                        }
                        else
                        {
                            metrics.get_hits++;
                            conn.outbuf += "$" + std::to_string(value.size()) + "\n";
                            conn.outbuf += value + "\n";
                        }
                        break;
                    }

                    case CmdType::DEL:
                    {
                        int removed = cache.del(cmd.key);
                        conn.outbuf += ":" + std::to_string(removed) + "\n";
                        break;
                    }

                    case CmdType::EXPIRE:
                    {
                        bool ok = cache.expire(cmd.key, std::stoll(cmd.value) * 1000);
                        conn.outbuf += ":" + std::to_string(ok) + "\n";
                        break;
                    }

                    case CmdType::TTL:
                    {
                        int64_t t = cache.ttl(cmd.key);
                        conn.outbuf += ":" + std::to_string(t) + "\n";
                        break;
                    }

                    case CmdType::STATS:
                    {
                        conn.outbuf += "requests " + std::to_string(metrics.requests.load()) + "\n";
                        conn.outbuf += "hits " + std::to_string(metrics.get_hits.load()) + "\n";
                        conn.outbuf += "misses " + std::to_string(metrics.get_misses.load()) + "\n";
                        conn.outbuf += "evictions " + std::to_string(metrics.evictions.load()) + "\n";
                        break;
                    }

                    default:
                        conn.outbuf += "-ERR unknown command\n";
                    }
                }

                if (!conn.outbuf.empty())
                {
                    write(ev.fd, conn.outbuf.data(), conn.outbuf.size());
                    conn.outbuf.clear();
                }
            }
        }
    }
}