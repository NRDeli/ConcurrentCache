#include "Reactor.h"

#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

Reactor::Reactor()
{
    epoll_fd = kqueue();
}

Reactor::~Reactor()
{
    close(epoll_fd);
}

void Reactor::add_fd(int fd, bool want_write)
{

    struct kevent ev;

    EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(epoll_fd, &ev, 1, NULL, 0, NULL);

    if (want_write)
    {
        EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        kevent(epoll_fd, &ev, 1, NULL, 0, NULL);
    }
}

void Reactor::mod_fd(int fd, bool want_write)
{

    struct kevent ev;

    if (want_write)
    {
        EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    }
    else
    {
        EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    }

    kevent(epoll_fd, &ev, 1, NULL, 0, NULL);
}

void Reactor::del_fd(int fd)
{

    struct kevent ev;

    EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(epoll_fd, &ev, 1, NULL, 0, NULL);

    EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    kevent(epoll_fd, &ev, 1, NULL, 0, NULL);
}

int Reactor::wait(std::vector<Event> &events, int timeout_ms)
{

    struct kevent evs[64];

    struct timespec ts;
    ts.tv_sec = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * 1000000;

    int n = kevent(epoll_fd, NULL, 0, evs, 64, &ts);

    for (int i = 0; i < n; i++)
    {

        Event e;
        e.fd = (int)evs[i].ident;
        e.readable = evs[i].filter == EVFILT_READ;
        e.writable = evs[i].filter == EVFILT_WRITE;

        events.push_back(e);
    }

    return n;
}