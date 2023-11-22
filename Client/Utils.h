#pragma once
#include <string>
#include <vector>

inline std::vector<std::string> split(std::string message, std::string delimiter)
{
    std::vector<std::string> mess;
    std::string str = message;
    size_t pos = 0;
    std::string token;

    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        mess.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    mess.push_back(str);
    return mess;
}
