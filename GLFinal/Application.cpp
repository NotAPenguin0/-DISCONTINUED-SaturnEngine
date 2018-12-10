#include "Application.hpp"
#include "ErrorHandler.hpp"
#include "Input.hpp"
#include "Time.hpp"
#include "log.hpp"
#include "RenderSystem.hpp"

namespace mvg {

Application::Application(WindowSettings winsettings) {
    // 1. Initialize GLFW to create window
    // 2. Create window
    // 3. Initialize GLAD to initialize GLFW

    if (!glfwInit()) {
        mvg::error("Failed to initialize GLFW");
    } else { // GLFW initialized successfully

        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        /*OS X needs this window hint of code to initialize*/
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        glfwSetErrorCallback(glfw_error_callback);

        // Uses move constructor, as a Window is NonCopyable
        window = Window(winsettings);

        // Error handling for failing window creation is already
        // handled in the Window constructor, so a simple check is enough here
        if (window.handle() != nullptr) {
            glfwMakeContextCurrent(window.handle());

            // Initialize GLAD
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                mvg::error("Failed to initialize GLAD");
                window.destroy();
                glfwTerminate();
            } else {
                glViewport(0, 0, winsettings.width, winsettings.height);
                glfwSetFramebufferSizeCallback(window.handle(),
                                               &Window::resize_callback);

                // Initialize the rest of the application
                init();
            }
        }
    }
}

void Application::init() {
    Input::bind(GLFW_KEY_ESCAPE, [this]() { quit(); });
    world.addSystem<Systems::RenderSystem>();
}

Application::~Application() {
    window.destroy();

    glfwTerminate();
}

void Application::run() {

	world.onStart();

    while (!glfwWindowShouldClose(window.handle())) {
        Time::update();
        Input::update(window);

		world.onUpdate();

        glfwSwapBuffers(window.handle());
        glfwPollEvents();
    }
}

void Application::quit() { glfwSetWindowShouldClose(window.handle(), true); }

} // namespace mvg
