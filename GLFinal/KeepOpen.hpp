#ifndef MVG_KEEP_OPEN_HPP_
#define MVG_KEEP_OPEN_HPP_

#include <iostream>

namespace mvg {

class KeepOpen {
public:
    inline ~KeepOpen() {
        std::cout << "---------------------------------------\n";
        std::cout << "Process terminated. Press ENTER to quit\n";
        std::cin.clear();
        std::cin.get();
    }
};

} // namespace mvg

#endif