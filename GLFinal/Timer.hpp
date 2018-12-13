#ifndef SATURN_TIMER_HPP_
#define SATURN_TIMER_HPP_

#include <chrono>

namespace Saturn
{

namespace chrono = ::std::chrono;

class Timer {
public:
    inline Timer() : start_point(chrono::system_clock::now()) {}

    template<typename Duration>
    void start(Duration d) {
        
        start_point = chrono::system_clock::now();
        duration = chrono::duration_cast<chrono::nanoseconds>(d);
    }

    inline bool has_ended() {
        return chrono::system_clock::now() - start_point >= duration;
    }

private:
    chrono::time_point<std::chrono::system_clock> start_point;
    chrono::nanoseconds duration;
};

}

#endif
