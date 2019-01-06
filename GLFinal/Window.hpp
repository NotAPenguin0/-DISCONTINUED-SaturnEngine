#ifndef SATURN_WINDOW_HPP_
#define SATURN_WINDOW_HPP_

#include "OpenGL.hpp"

#include <GLM\glm.hpp>

#include <string>

namespace Saturn {

class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(NonCopyable const&) = delete;
    NonCopyable& operator=(NonCopyable const&) = delete;
};

struct WindowSettings {
    WindowSettings(std::string c, std::size_t w, std::size_t h);
    WindowSettings() = default;

    std::string caption;
    std::size_t width;
    std::size_t height;

    bool fullscreen = false;
};

class Window : public NonCopyable {
public:
    // To be able to access the callback functions
    friend class Application;

    Window();
    Window(WindowSettings settings);
    Window(Window&&);
    Window& operator=(Window&&);
    ~Window();

    GLFWwindow* handle();

    bool is_open() const;

    void destroy();

    void clear(glm::vec3 color,
               GLenum buffers = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    void set_position(float x, float y);

    std::size_t width() const;
    std::size_t height() const;
    std::string caption() const;
    bool fullscreen() const;
    WindowSettings settings() const;

    void set_fullscreen(bool f);

private:
    static void
    resize_callback([[maybe_unused]] GLFWwindow* window, int w, int h);

    GLFWwindow* win;
    WindowSettings wsettings;
};

} // namespace Saturn

#endif
