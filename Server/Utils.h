#pragma once
#include <string>
#include <vector>

inline bool checkPort(std::string s) {
    if (s.length() < 4 || s.length() > 4) return false;
    for( int i = 0; i < s.length(); i++ ) {
        if(!isdigit( s[i])) {
            return false;
        }
    }
    return true;
}