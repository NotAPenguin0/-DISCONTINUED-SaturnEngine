#include "Time.hpp"

#include "OpenGL.hpp"

namespace mvg {

void Time::update() {
    auto currFrame = static_cast<float>(glfwGetTime());
    deltaTime = currFrame - lastFrame;
    lastFrame = currFrame;
}

float Time::now()
{ return static_cast<float>(glfwGetTime()); }

} // namespace mvg
