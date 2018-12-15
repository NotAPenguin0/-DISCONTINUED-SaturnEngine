#include "Application.hpp"
#include "ErrorHandler.hpp"
#include "Globals.hpp"
#include "Input.hpp"
#include "RenderSystem.hpp"
#include "Time.hpp"
#include "log.hpp"

namespace Saturn {

Application::Application(WindowSettings winsettings) :
    modelManager(ResourceLoaders::loadModel, ResourceLoaders::unloadModel),
    textureManager(ResourceLoaders::loadTexture,
                   ResourceLoaders::unloadTexture),
    shaderManager(ResourceLoaders::loadShader, ResourceLoaders::unloadShader) {
    // 1. Initialize GLFW to create window
    // 2. Create window
    // 3. Initialize GLAD to initialize GLFW

    if (!glfwInit()) {
        Saturn::error("Failed to initialize GLFW");
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
                Saturn::error("Failed to initialize GLAD");
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

void Application::mouse_callback([[maybe_unused]] GLFWwindow* win,
                                 double xpos,
                                 double ypos) {
    static bool firstMouse = true;
    static float lastX = gWindow->width() / 2;
    static float lastY = gWindow->height() / 2;

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

    gCamera->mouse_move(xoffset, yoffset);
}

void Application::scroll_callback([[maybe_unused]] GLFWwindow* window,
                                  [[maybe_unused]] double xoffset,
                                  double yoffset) {

    gCamera->zoom(yoffset);
}

void Application::init() {
    // Set globals
    gApp = this;
    gWorld = &world;
    gCamera = &camera;
    gWindow = &window;

	//Setup camera variables
    camera.Speed = 5.0f;
    camera.Sensitivity = 0.1f;
    camera.Position = glm::vec3(0.0f, 0.0f, 2.0f);
    // Bind input keys
    glfwSetInputMode(window.handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input::bind(GLFW_KEY_ESCAPE, [this]() { quit(); });
    Input::bind(GLFW_KEY_P, [this]() {
        std::cout << "Current position:" << camera.Position << "\n";
    });
    // Setup camera movement
    Input::setMouseCallback(&Application::mouse_callback);
    Input::setScrollCallback(&Application::scroll_callback);
    Input::bind(GLFW_KEY_W,
                [this]() { camera.move(Direction::Forward, Time::deltaTime); });
    Input::bind(GLFW_KEY_S, [this]() {
        camera.move(Direction::Backward, Time::deltaTime);
    });
    Input::bind(GLFW_KEY_A,
                [this]() { camera.move(Direction::Left, Time::deltaTime); });
    Input::bind(GLFW_KEY_D,
                [this]() { camera.move(Direction::Right, Time::deltaTime); });
    Input::bind(GLFW_KEY_SPACE,
                [this]() { camera.move(Direction::Up, Time::deltaTime); });
    Input::bind(GLFW_KEY_LEFT_SHIFT,
                [this]() { camera.move(Direction::Down, Time::deltaTime); });
    // Set systems
    world.addSystem<Systems::RenderSystem>();

    // Temporary object for testing purposes
    auto& objects = world.objects;

    auto& cube = objects.emplace();
    cube.addComponent<Components::Mesh>();
    cube.addComponent<Components::Transform>();
    cube.addComponent<Components::Shader>();
    auto& model = cube.getComponent<Components::Mesh>();
    auto& trans = cube.getComponent<Components::Transform>();
    auto& shader = cube.getComponent<Components::Shader>();
    shader.shader = shaderManager.get("resources/shaders/default.sh");
    model.model = modelManager.get("resources/temp/vertices.txt");
    trans.position = glm::vec3(0.0f, 0.0f, 0.0f);
    trans.rotation = {glm::vec3(1.0f), 0.0f};
    trans.scale = glm::vec3(1.0f);
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

} // namespace Saturn
