#pragma once

#include <string>
#include <vector>

enum class CmdType
{
    PING,
    SET,
    GET,
    DEL,
    INVALID
};

struct Command
{
    CmdType type;
    std::string key;
    std::string value;
};

class Parser
{
public:
    static Command parse(const std::string &line);

private:
    static std::vector<std::string> split(const std::string &line);
};