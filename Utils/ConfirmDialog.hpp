#pragma once

#include <iostream>
#include <string>

inline bool confirm(const std::string& promt) {
    if (!promt.empty())
        std::cout << promt << " [Yy/Nn]" << std::endl;
    while (true) {
        unsigned char sym = std::cin.get();
        if (sym == 'y' || sym == 'Y')
            return true;
        else if (sym == 'n' || sym == 'N')
            return false;
        else
            continue;
    }
}
