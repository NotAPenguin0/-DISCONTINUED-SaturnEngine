#include "Input.hpp"

#include "Window.hpp"

namespace mvg {

std::unordered_map<Input::KeyT, Input::CallbackT> Input::keybinds;

void Input::update(Window& win) {
    for (auto& [k, cb] : keybinds) {
        if (glfwGetKey(win.handle(), k) == GLFW_PRESS) {
            // Call the callback function the user specified
            cb();
        }
    }
}

bool Input::key_pressed(Window& win, KeyT key)
{
    return glfwGetKey(win.handle(), key) == GLFW_PRESS;
}

} // namespace mvg
