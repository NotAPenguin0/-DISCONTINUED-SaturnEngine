#ifndef MVG_TIME_HPP_
#define MVG_TIME_HPP_

namespace mvg {

class Time {
public:
    static inline float deltaTime;
    static float now();

private:
    friend class Application;

	static void update();

    static inline float lastFrame;
};

} // namespace mvg

#endif