#include "initgl.hpp"

#include "Input.hpp"
#include "log.hpp"

Saturn::FPSCamera camera;
bool firstMouse = true;
float lastX = 400, lastY = 300;

void init() {
    if (!glfwInit()) { Saturn::error("Failed to initialize GLFW\n"); }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    /*OS X needs this line of code to initialize*/
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window,
                               int w,
                               int h) {
    glViewport(0, 0, w, h);
}

void mouse_callback([[maybe_unused]] GLFWwindow* window,
                    double xpos,
                    double ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    // reversed since y-coordinates go from bottom to top
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.mouse_move(xoffset, yoffset);
}

void scroll_callback([[maybe_unused]] GLFWwindow* window,
                     [[maybe_unused]] double xoffset,
                     double yoffset) {
    camera.zoom(yoffset);
}

Saturn::Window init_all() {
    init();

    // make window here and ensure active context
    Saturn::Window window(Saturn::WindowSettings{"OpenGL Engine", 800, 600});

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Saturn::error("Failed to initialize GLAD");
        window.destroy();
        glfwTerminate();
        throw std::runtime_error("Failed to initialize");
    }

    glViewport(0, 0, 800, 600);

    Saturn::Input::setScrollCallback(window, scroll_callback);
    Saturn::Input::setMouseCallback(window, mouse_callback);

    return window;
}