#ifndef SATURN_KEEP_OPEN_HPP_
#define SATURN_KEEP_OPEN_HPP_

#include <iostream>

namespace Saturn {

class KeepOpen {
public:
    inline ~KeepOpen() {
        std::cout << "---------------------------------------\n";
        std::cout << "Process terminated. Press ENTER to quit\n";
        std::cin.clear();
        std::cin.get();
    }
};

} // namespace Saturn

#endif