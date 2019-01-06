#ifndef MVG_FPS_COUNTER_HPP_
#define MVG_FPS_COUNTER_HPP_

#include <chrono>

class FPSCounter {
public:
    static float fps();
    static std::chrono::milliseconds frameTime();
};

#endif