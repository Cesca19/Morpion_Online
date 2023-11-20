﻿#pragma once

#include <iostream>
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
    std::cout << str << std::endl;
    return mess;
}

inline int** convertStringBoard(std::string mess)
{
    int **map = new int* [3];
    for (int i = 0, j = 0; i < 3 && j < mess.size(); i++) {
        map[i] = new int[3];
        map[i][0] = mess[j] - 48; j++;
        map[i][1] = mess[j] - 48; j++;
        map[i][2] = mess[j] - 48; j++;
    }
    return map;
}

inline std::string convertBoard(int** board)
{
    std::string mess;
    for (int i = 0; i < 3; i++) {
        mess += board[i][0] + '0';
        mess += board[i][1] + '0';
        mess += board[i][2] + '0';
    }
    return mess;
}