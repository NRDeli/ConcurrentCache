#include "Parser.h"
#include <sstream>

std::vector<std::string> Parser::split(const std::string &line)
{

    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string item;

    while (ss >> item)
        tokens.push_back(item);

    return tokens;
}

Command Parser::parse(const std::string &line)
{

    auto tokens = split(line);

    if (tokens.empty())
        return {CmdType::INVALID, "", ""};

    if (tokens[0] == "PING")
        return {CmdType::PING, "", ""};

    if (tokens[0] == "SET" && tokens.size() >= 3)
        return {CmdType::SET, tokens[1], tokens[2]};

    if (tokens[0] == "GET" && tokens.size() >= 2)
        return {CmdType::GET, tokens[1], ""};

    if (tokens[0] == "DEL" && tokens.size() >= 2)
        return {CmdType::DEL, tokens[1], ""};

    if (tokens[0] == "EXPIRE" && tokens.size() >= 3)
        return {CmdType::EXPIRE, tokens[1], tokens[2]};

    if (tokens[0] == "TTL" && tokens.size() >= 2)
        return {CmdType::TTL, tokens[1], ""};

    if (tokens[0] == "STATS")
        return {CmdType::STATS, "", ""};

    return {CmdType::INVALID, "", ""};
}