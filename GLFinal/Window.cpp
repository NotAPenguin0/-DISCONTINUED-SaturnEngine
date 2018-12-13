#include "Window.hpp"
#include "log.hpp"

#include <utility>

namespace Saturn {

WindowSettings::WindowSettings(std::string c, std::size_t w, std::size_t h) :
    caption(c), width(w), height(h) {}

Window::Window() : win(nullptr) {}

Window::Window(WindowSettings settings) : win(nullptr) {
    win = glfwCreateWindow(
        settings.width, settings.height, settings.caption.c_str(),
        settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    if (win == nullptr) {
        Saturn::error("FATAL ERROR: Failed to create window");
    } else {
        wsettings = settings;
    }
}

Window::Window(Window&& rhs) : win(rhs.win), wsettings(rhs.wsettings) {
    rhs.win = nullptr;
    rhs.wsettings = WindowSettings();
}

Window& Window::operator=(Window&& rhs) {
    if (win != rhs.win) {
        std::swap(win, rhs.win);
        std::swap(wsettings, rhs.wsettings);
    }
    return *this;
}

Window::~Window() { destroy(); }

GLFWwindow* Window::handle() { return win; }

void Window::destroy() {
    if (win != nullptr) { glfwDestroyWindow(win); }
    win = nullptr;
}

bool Window::is_open() const {
    return win != nullptr && !glfwWindowShouldClose(win);
}

void Window::resize_callback([[maybe_unused]] GLFWwindow* window,
                             int w,
                             int h) {
    glViewport(0, 0, w, h);
}

std::size_t Window::width() const
{ return wsettings.width; }

std::size_t Window::height() const
{ return wsettings.height; }

std::string Window::caption() const
{ return wsettings.caption; }

bool Window::fullscreen() const
{ return wsettings.fullscreen; }

WindowSettings Window::settings() const
{ return wsettings; }

void Window::set_fullscreen(bool f) {
    glfwSetWindowMonitor(win, f ? glfwGetPrimaryMonitor() : nullptr, 0, 0,
                         wsettings.width, wsettings.height, GLFW_DONT_GIVE_A_SHIT);
    wsettings.fullscreen = f;
}

} // namespace Saturn
