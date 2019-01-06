#include "Application.hpp"
#include "ErrorHandler.hpp"
#include "Globals.hpp"
#include "Input.hpp"
#include "RenderSystem.hpp"
#include "CameraLightSystem.hpp"
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

    // Setup camera variables
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
    Input::setMouseCallback(window, &Application::mouse_callback);
    Input::setScrollCallback(window, &Application::scroll_callback);
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
	world.addSystem<Systems::CameraLightSystem>();

    // Set predefined shaders
    defaultShader = shaderManager.get("resources/shaders/default.sh");
    texturedShader = shaderManager.get("resources/shaders/textured.sh");
    lightShader = shaderManager.get("resources/shaders/light.sh");

    // Temporary object for testing purposes
    auto& objects = world.objects;

    auto& cube = objects.emplace();
    cubeId = cube.id();
    cube.addComponent<Components::Mesh>();
    cube.addComponent<Components::Transform>();
    cube.addComponent<Components::Shader>();
    cube.addComponent<Components::Material>();
    auto& model = cube.getComponent<Components::Mesh>();
    auto& trans = cube.getComponent<Components::Transform>();
    auto& shader = cube.getComponent<Components::Shader>();
    auto& material = cube.getComponent<Components::Material>();
    shader.shader = lightShader;
    model.model = modelManager.get("resources/temp/vertices.txt");
    //    material.texture = textureManager.get("resources/temp/container.tex");
    material.lighting.specularMap =
        textureManager.get("resources/temp/container2_specular.tex");
    material.lighting.diffuseMap =
        textureManager.get("resources/temp/container.tex");
    material.lighting.shininess = 64.0f;
    material.lighting.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
    trans.position = glm::vec3(0.0f, 0.0f, 0.0f);
    trans.rotation = {glm::vec3(1.0f), 0.0f};
    trans.scale = glm::vec3(1.0f);

    // Lighting
    glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    /* // Create a directional light
     DirectionalLight dirLight;
     dirLight.ambient = ambient;
     dirLight.diffuse = diffuse;
     dirLight.specular = specular;
     dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);

     // std::move because we don't need this dirLight anymore
     lights.directionalLights.push_back(std::move(dirLight));

     PointLight pointLight;
     pointLight.position = glm::vec3(0.5f, 0.5f, -2.0f);
     pointLight.ambient = ambient;
     pointLight.diffuse = diffuse;
     pointLight.specular = specular;
     pointLight.linear = linear;
     pointLight.constant = constant;
     pointLight.quadratic = quadratic;

     lights.pointLights.push_back(std::move(pointLight));

     auto& ptLight = objects.emplace();
     ptLight.addComponent<Components::Mesh>();
     ptLight.addComponent<Components::Transform>();
     ptLight.addComponent<Components::Shader>();
     ptLight.addComponent<Components::Material>();

     ptLight.getComponent<Components::Shader>().shader = defaultShader;
     ptLight.getComponent<Components::Transform>().position =
         glm::vec3(0.7f, 0.2f, -5.0f);
     ptLight.getComponent<Components::Transform>().scale =
         glm::vec3(0.5f, 0.5f, 0.5f);
     // The point light and cube use the same mesh

     ptLight.getComponent<Components::Mesh>().model =
         modelManager.get("resources/temp/lamp.txt");
	*/
     SpotLight spotLight;
     spotLight.position = camera.Position;
     spotLight.direction = camera.Front;
     spotLight.ambient = glm::vec3(0.0f);
     spotLight.diffuse = glm::vec3(1.0f);
     spotLight.specular = glm::vec3(1.0f);
     spotLight.linear = linear;
     spotLight.constant = constant;
     spotLight.quadratic = quadratic;
     spotLight.radius = glm::radians(12.5f);
     spotLight.soft_radius = glm::radians(15.0f);

    auto camLight = world.objects.emplace();
    // Set reference to camera
    camLight.addComponent<Components::CamLight>();
    camLight.getComponent<Components::CamLight>().camera = &camera;

    camLight.addComponent<Components::Lights>();
    auto& camLights = camLight.getComponent<Components::Lights>();
    camLights.spotLights.push_back(std::move(spotLight));
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

        // Update world (and systems)
        world.onUpdate();

        glfwSwapBuffers(window.handle());
        glfwPollEvents();
    }
}

void Application::quit() { glfwSetWindowShouldClose(window.handle(), true); }

} // namespace Saturn
