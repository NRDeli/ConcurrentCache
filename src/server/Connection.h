#pragma once

#include <string>

class Connection
{
public:
    int fd;
    std::string inbuf;
    std::string outbuf;
    bool closed = false;

    explicit Connection(int fd_);
};