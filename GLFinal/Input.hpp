#ifndef MVG_INPUT_HPP_
#define MVG_INPUT_HPP_

#include <functional>
#include <unordered_map>

#include "OpenGL.hpp"


namespace mvg {

class Window;

class Input {
public:
    using CallbackT = std::function<void()>;
    using KeyT = int;

    template<typename Callback>
	static void bind(KeyT key, Callback&& func) {
        keybinds[key] = std::forward<Callback>(func);
	}

	static bool key_pressed(Window& win, KeyT key);

private:
    friend class Application;

    static void update(Window& win);

    static std::unordered_map<KeyT, CallbackT> keybinds;
};

} // namespace mvg

#endif