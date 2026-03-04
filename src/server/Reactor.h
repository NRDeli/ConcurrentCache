#pragma once

#include <vector>

struct Event
{
    int fd;
    bool readable;
    bool writable;
};

class Reactor
{
public:
    Reactor();
    ~Reactor();

    void add_fd(int fd, bool want_write);
    void mod_fd(int fd, bool want_write);
    void del_fd(int fd);

    int wait(std::vector<Event> &events, int timeout_ms);

private:
    int epoll_fd;
};