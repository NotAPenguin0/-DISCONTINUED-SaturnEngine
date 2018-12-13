#ifndef SATURN_INPUT_HPP_
#define SATURN_INPUT_HPP_

#include <functional>
#include <unordered_map>

#include "Globals.hpp"
#include "OpenGL.hpp"

namespace Saturn {

class Window;

class Input {
public:
    enum class Action { Scroll, MouseMove, Keypress };

    using CallbackT = std::function<void()>;
    using KeyT = int;

    template<typename Callback>
    static void bind(KeyT key, Callback&& func) {
        keybinds[key] = std::forward<Callback>(func);
    }

    // Callback should void(GLFWWindow*, double, double)
    template<typename Callback>
    static void setMouseCallback(Callback&& cb) {
        glfwSetCursorPosCallback(gWindow->handle(), cb);
    }

	//Callback should be void(GLFWWindow*, double, double)
    template<typename Callback>
    static void setScrollCallback(Callback&& cb) {
        glfwSetScrollCallback(gWindow->handle(), cb);
    }

    static bool key_pressed(Window& win, KeyT key);

private:
    friend class Application;

    static void update(Window& win);

    static std::unordered_map<KeyT, CallbackT> keybinds;
};

} // namespace Saturn

#endif
