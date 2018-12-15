#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <array>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "FPSCamera.hpp"
#include "Light.hpp"
#include "LightShader.hpp"
#include "Texture.hpp"
#include "Timer.hpp"
#include "depend/stb_image.h"
#include "log.hpp"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#define UNUSED(x) (void)x

static const char* VertexPath = "shaders/vertex.glsl";
static const char* FragmentPath = "shaders/fragment.glsl";
static const char* LampVertex = "shaders/lamp_vertex.glsl";
static const char* LampFragment = "shaders/lamp_fragment.glsl";
unsigned int shader; // global, yeah bad practice, fuck you this entire file is
                     // bad practice so who cares about a global
unsigned int lampShader;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
Saturn::FPSCamera camera(glm::vec3(0.5, 1.0f, 4.5f));

std::ostream& operator<<(std::ostream& out, glm::vec3 const& vec);

std::istream& operator>>(std::istream& in, glm::vec3& vec) {
    in >> vec.x >> vec.y >> vec.z;
    return in;
}

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



void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

unsigned int create_shader(const char* vtx_path, const char* frag_path) {
    using namespace std::literals::string_literals;

    std::fstream file(vtx_path);

    if (!file.good()) {
        Saturn::error(
            "[SHADER::VERTEX]: failed to open vertex shader source file at path"s +
            vtx_path);
        return -1;
    }

    std::stringstream buf;
    buf << file.rdbuf();

    std::string vtx_source(buf.str());

    file.close();
    buf = std::stringstream{}; // reset buffer
    file.open(frag_path);

    if (!file.good()) {
        Saturn::error(
            "[SHADER::FRAGMENT]: failed to open fragment shader source file at path"s +
            frag_path);
        return -1;
    }

    buf << file.rdbuf();

    std::string frag_source(buf.str());

    unsigned int vtx_shader, frag_shader;
    vtx_shader = glCreateShader(GL_VERTEX_SHADER);
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // This is wrapped inside a lambda to limit the scope of vtx_carr and
    // frag_carr
    [&vtx_source, &frag_source, &vtx_shader, &frag_shader]() {
        const char* vtx_carr = vtx_source.c_str();
        const char* frag_carr = frag_source.c_str();
        glShaderSource(vtx_shader, 1, &vtx_carr, nullptr);
        glShaderSource(frag_shader, 1, &frag_carr, nullptr);
    }();

    glCompileShader(vtx_shader);
    glCompileShader(frag_shader);

    // Now, check for compilation errors
    int success;
    char infolog[512];
    glGetShaderiv(vtx_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vtx_shader, 512, nullptr, infolog);
        Saturn::error("[SHADER::VERTEX::COMPILATION_FAILED]: "s + infolog);
        return -1;
    }

    // And again for the fragment shader
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, nullptr, infolog);
        Saturn::error("[SHADER::FRAGMENT::COMPILATION_FAILED]: "s + infolog);
        return -1;
    }

    // Finally, link the vertex and fragment shader together
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vtx_shader);
    glAttachShader(shaderProgram, frag_shader);
    glLinkProgram(shaderProgram);

    // Check for errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infolog);
        Saturn::error("[SHADER::LINK_FAILED]: "s + infolog);
        return -1;
    }

    // These are linked now and can safely be deleted
    glDeleteShader(vtx_shader);
    glDeleteShader(frag_shader);

    return shaderProgram;
}

void reload_shader(unsigned int& prog, const char* vtx, const char* frag) {
    prog = create_shader(vtx, frag);
}

void process_input(GLFWwindow* window) {
    static Saturn::Timer shaderReloadTimer;
    static Saturn::Timer wireFrameTimer;
    static Saturn::Timer posTimer;
    static bool wireFrameEnabled = false;
    static constexpr std::chrono::milliseconds delay{300};

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.Speed = 10.0f;
    } else {
        camera.Speed = 5.0f;
    }

    using Saturn::Direction;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.move(Direction::Forward, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.move(Direction::Backward, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.move(Direction::Left, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.move(Direction::Right, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.move(Direction::Up, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.move(Direction::Down, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            if (shaderReloadTimer.has_ended()) {
                shaderReloadTimer.start(delay);
                reload_shader(shader, VertexPath, FragmentPath);
                reload_shader(lampShader, LampVertex, LampFragment);
                Saturn::info("Shader reload complete");
            }
        }
        if (glfwGetKey(window, GLFW_KEY_Z) ==
            GLFW_PRESS) { // GLFW_KEY_Z because I have an Azerty keyboard, which
                          // means Z corresponds to W
            if (wireFrameTimer.has_ended()) {
                wireFrameTimer.start(delay);
                wireFrameEnabled = !wireFrameEnabled;

                if (wireFrameEnabled) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    Saturn::info("Wireframe mode enabled");
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    Saturn::info("Wireframe mode disabled");
                }
            }
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            if (posTimer.has_ended()) {
                posTimer.start(delay);
                Saturn::debug("current camera position: ");
                std::cout << camera.Position << "\n";
            }
        }
    }
}

glm::mat4 identity() { return glm::mat4(1.0); }

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    UNUSED(window);

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    UNUSED(window);
    UNUSED(xoffset);
    camera.zoom(yoffset);
}

GLFWwindow* init_all() {
    init();

    // make window here and ensure active context
    GLFWwindow* window = glfwCreateWindow(
        800, 600, "LearnOpenGL for the final time!", nullptr, nullptr);

    if (!window) {
        Saturn::error("Failed to create window");
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Saturn::error("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    return window;
}

int main2() {


    auto window = init_all();

    // shader = create_shader(VertexPath, FragmentPath);
    lampShader = create_shader(LampVertex, LampFragment);
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
        -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
        0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
        1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
        0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
        0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
        -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
        0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
        0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
        -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
        0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
        -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
        -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
        1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
        -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
        1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, -20.0f), glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, -1.0f, -7.0f)};

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind buffer and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int lampVAO;
    glGenVertexArrays(1, &lampVAO);
    glBindVertexArray(lampVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    glm::mat4 model = identity();

    glm::mat4 view = identity();

    glm::mat4 proj = identity();

    glEnable(GL_DEPTH_TEST);

    Saturn::Texture container;
    container.load("container2.png", GL_TEXTURE0, GL_RGBA);
    Saturn::Texture cont_specular;
    cont_specular.load("container2_specular.png", GL_TEXTURE1, GL_RGBA);

    /*   auto materials = load_materials("materials.txt");
       auto& mat = materials.at("gold");

       glm::vec3 objColor = mat.color;*/
    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

    glm::vec3 light_ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 light_diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    glm::vec3 light_specular = glm::vec3(0.5f, 0.5f, 0.5f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    lightPos.y = 0.0f;

    //    float spotLightAngle = 12.5f;
    //    float spotLightOuterAngle = 17.5f;

    Saturn::deleted::LightShader ls;
    Saturn::DirectionalLight light;
    Saturn::SpotLight camLight;
    Saturn::Material mat;

    camLight.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    camLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    camLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    camLight.position = camera.Position;
    camLight.direction = camera.Front;
    camLight.constant = 1.0f;
    camLight.linear = 0.09f;
    camLight.quadratic = 0.032f;
    camLight.radius = glm::radians(12.5f);
    camLight.soft_radius = glm::radians(15.0f);

    ls.add<Saturn::SpotLight>(&camLight);

    std::array<Saturn::PointLight, 4> ptLights;
    for (int i = 0; i < 4; ++i) {
        auto& l = ptLights[i];
        l.position = pointLightPositions[i];
        l.ambient = light_ambient;
        l.diffuse = light_diffuse;
        l.specular = light_specular;
        l.linear = linear;
        l.constant = constant;
        l.quadratic = quadratic;
        ls.add<Saturn::PointLight>(&l);
    }

    float shininess = 64.0f;
    UNUSED(shininess);
    mat.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
    mat.diffuseMap = &container;
    mat.specularMap = &cont_specular;
    mat.shininess = 64.0f;
    light.ambient = light_ambient;
    light.diffuse = light_diffuse;
    light.specular = light_specular;
    light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    ls.add<Saturn::DirectionalLight>(&light);
    ls.material = mat;
	ls.camera = &camera;

	
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        process_input(window);

        ls.use();

		camLight.direction = camera.Front;
        camLight.position = camera.Position;


        model = identity();
        view = camera.view_matrix();
        proj = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f,
                                0.1f, 100.0f);

        ls.view = view;
        ls.projection = proj;

        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

        light.diffuse = lightColor * glm::vec3(0.5); // decrease the influence
        light.ambient = light_diffuse * glm::vec3(0.2); // low influence

        glBindVertexArray(VAO);
        // Draw main cubes
        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader
            // before drawing
            container.bind();
            cont_specular.bind();

            model = identity();
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle),
                                glm::vec3(1.0f, 0.3f, 0.5f));
            ls.model = model;
            ls.update_uniforms();

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Draw light cube
        glBindVertexArray(lampVAO);
        glUseProgram(lampShader);
        model = identity();

        glUniformMatrix4fv(glGetUniformLocation(lampShader, "view"), 1,
                           GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lampShader, "proj"), 1,
                           GL_FALSE, glm::value_ptr(proj));
        for (auto pointLightPosition : pointLightPositions) {
            model = identity();
            model = glm::scale(model, glm::vec3(0.2f));
            model = glm::translate(model, pointLightPosition);

            glUniformMatrix4fv(glGetUniformLocation(lampShader, "model"), 1,
                               GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lampVAO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
