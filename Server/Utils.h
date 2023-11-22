#pragma once
#include <string>
#include <vector>

inline bool isNumber(std::string s) {
    for( int i = 0; i < s.length(); i++ ) {
        if( !isdigit( s[i] )) {
            return false;
        }
    }
    return true;
}