#pragma once
#include <iostream>

class Logger
{
public:
    template <typename T>
    static void info(const T &msg)
    {
        std::cout << "[INFO] " << msg << std::endl;
    }

    template <typename T>
    static void error(const T &msg)
    {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
};