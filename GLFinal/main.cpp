#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <array>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Cube.hpp"
#include "FPSCamera.hpp"
#include "Model.hpp"
#include "Plane.hpp"
#include "Texture.hpp"
#include "depend/stb_image.h"
#include "log.hpp"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

namespace old {

#define UNUSED(x) (void)x

float plane_yoffset = 0;

#pragma region helpers

#ifdef _DEBUG
inline void check_gl_errors() {
    GLenum error = glGetError();
    if (error == GL_INVALID_ENUM) {
        throw std::runtime_error("GL_INVALID_ENUM");
    } else if (error == GL_INVALID_VALUE) {
        throw std::runtime_error("GL_INVALID_VALUE");
    } else if (error == GL_INVALID_OPERATION) {
        throw std::runtime_error("GL_INVALID_OPERATION");
    } else if (error == GL_INVALID_FRAMEBUFFER_OPERATION) {
        throw std::runtime_error("GL_INVALID_FRAMEBUFFER_OPERATION");
    } else if (error == GL_OUT_OF_MEMORY) {
        throw std::runtime_error("GL_OUT_OF_MEMORY");
    }
}
#    define CHECK_GL_ERRORS() check_gl_errors()
#else
#    define CHECK_GL_ERRORS()
#endif // _DEBUG

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
::Saturn::FPSCamera camera(glm::vec3(0.5, 1.0f, 4.5f));

std::ostream& operator<<(std::ostream& out, glm::vec3 const& vec) {
    return out << vec.x << " " << vec.y << " " << vec.z;
}

std::istream& operator>>(std::istream& in, glm::vec3& vec) {
    in >> vec.x >> vec.y >> vec.z;
    return in;
}

void init() {
    if (!glfwInit()) { ::Saturn::error("Failed to initialize GLFW\n"); }
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

unsigned int create_shader(const char* vtx_path, const char* frag_path) {
    using namespace std::literals::string_literals;

    std::fstream file(vtx_path);

    if (!file.good()) {
        ::Saturn::error(
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
        ::Saturn::error(
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
        ::Saturn::error("Failed to compile vertex shader at path: "s +
                        vtx_path);
        ::Saturn::error("[SHADER::VERTEX::COMPILATION_FAILED]: "s + infolog);
        return -1;
    }

    // And again for the fragment shader
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, nullptr, infolog);
        ::Saturn::error("Failed to compile fragment shader at path: "s +
                        frag_path);
        ::Saturn::error("[SHADER::FRAGMENT::COMPILATION_FAILED]: "s + infolog);
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
        ::Saturn::error("Failed to link shader. Vertex\n: "s + vtx_path +
                        "\nFragment: "s + frag_path);
        ::Saturn::error("[SHADER::LINK_FAILED]: "s + infolog);
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

    static bool wireFrameEnabled = false;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.Speed = 10.0f;
    } else {
        camera.Speed = 5.0f;
    }

    using ::Saturn::Direction;

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

            reload_shader(shader, VertexPath, FragmentPath);
            reload_shader(lampShader, LampVertex, LampFragment);
            ::Saturn::info("Shader reload complete");
        }
        if (glfwGetKey(window, GLFW_KEY_Z) ==
            GLFW_PRESS) { // GLFW_KEY_Z because I have an Azerty keyboard, which
                          // means Z corresponds to W

            wireFrameEnabled = !wireFrameEnabled;

            if (wireFrameEnabled) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                ::Saturn::info("Wireframe mode enabled");
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                ::Saturn::info("Wireframe mode disabled");
            }
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {

            ::Saturn::debug("current camera position: ");
            std::cout << camera.Position << "\n";
        }
    }
    float plane_speed = 0.02f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        plane_yoffset -= plane_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        plane_yoffset += plane_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        std::cout << "Plane y offset: " << plane_yoffset << "\n";
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
        ::Saturn::error("Failed to create window");
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ::Saturn::error("Failed to initialize GLAD");
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

#include "KeepOpen.hpp"

unsigned int load_cubemap(std::string directory,
                          std::vector<std::string> const& faces) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    int w, h, channels;
    for (std::size_t i = 0; i < faces.size(); ++i) {
        unsigned char* data =
            stbi_load((directory + faces[i]).c_str(), &w, &h, &channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            ::Saturn::error("Failed to load cubemap texture at path: " +
                            faces[i]);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return tex;
}

#pragma endregion helpers

int main2() {
    srand(time(nullptr));
    rand(); // first might not be random on all compilers

    ::Saturn::log("Starting initialization");

    auto window = init_all();

    glfwSetWindowPos(window, 550, 100);

    ::Saturn::log("Loading shaders");

    shader = create_shader("resources/shaders/scene_tex_def_v.glsl",
                           "resources/shaders/scene_tex_def_f.glsl");

    auto grass_shader = create_shader("resources/shaders/grass_v.glsl",
                                      "resources/shaders/grass_f.glsl");

    auto window_shader = create_shader("resources/shaders/window_v.glsl",
                                       "resources/shaders/window_f.glsl");

    auto framebuffer_shader =
        create_shader("resources/shaders/framebuffer_v.glsl",
                      "resources/shaders/framebuffer_f.glsl");

    auto skybox_shader = create_shader("resources/shaders/skybox_v.glsl",
                                       "resources/shaders/skybox_f.glsl");

    auto mirror_shader = create_shader("resources/shaders/mirror_v.glsl",
                                       "resources/shaders/mirror_f.glsl");

    auto mod_s = create_shader("resources/shaders/model_vtx.glsl",
                               "resources/shaders/model_frag.glsl");

    ::Saturn::log("Loading textures");

    ::Saturn::Texture grass;
    grass.load("resources/textures/grass.png", GL_TEXTURE0, GL_RGBA);
    ::Saturn::Texture floor;
    floor.load("resources/textures/marble.jpg", GL_TEXTURE0, GL_RGB);
    ::Saturn::Texture cube_tex;
    cube_tex.load("resources/textures/wood.png", GL_TEXTURE0, GL_RGBA);
    ::Saturn::Texture window_tex;
    window_tex.load("resources/textures/window.png", GL_TEXTURE0, GL_RGBA);

    // http://www.humus.name/index.php?page=Textures is a nice website for
    // skyboxes
    std::vector<std::string> faces = {"negx.jpg", "posx.jpg", "posy.jpg",
                                      "negy.jpg", "negz.jpg", "posz.jpg"};
    unsigned int cubemap_tex =
        load_cubemap("resources/skyboxes/skybox/", faces);

    ::Saturn::log("Loading models");

    Model nanosuit =
        Model::load_from_file("resources/models/nanosuit/nanosuit.obj");

    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
        0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
        0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
        0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
        0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
        -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
        -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
        1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
        1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
        0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
        0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
        0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
        0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};
    float planeVertices[] = {
        // positions          // texture Coords
        5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, 5.0f,
        0.0f, 0.0f,  -5.0f, -0.5f, -5.0f, 0.0f,  2.0f,

        5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
        0.0f, 2.0f,  5.0f,  -0.5f, -5.0f, 2.0f,  2.0f};
    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because
        // texture is flipped upside down)
        0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -0.5f, 0.0f,
        0.0f, 1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 1.0f, -0.5f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f};

    float screenVertices[] = {
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // TL
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // BL
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // BR
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // Second triangle TL
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // TR
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // Second triangle BR
    };

    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    std::vector<glm::vec3> grass_plants;
    grass_plants.emplace_back(-1.5f, 0.0f, -0.48f);
    grass_plants.emplace_back(1.5f, 0.0f, 0.51f);
    grass_plants.emplace_back(0.0f, 0.0f, 0.7f);
    grass_plants.emplace_back(-0.3f, 0.0f, -2.3f);
    grass_plants.emplace_back(0.5f, 0.0f, -0.6f);

    ::Saturn::log("Allocating buffers");

    Cube cube;
    Plane floor_plane;

    // transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices),
                 transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int screenVAO, screenVBO;
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glBindVertexArray(0);

    // Create the framebuffer

    unsigned int fbo; // frame buffer object
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Setup framebuffer

    // Create a texture to render to
    unsigned int buf_texture;
    glGenTextures(1, &buf_texture);
    glBindTexture(GL_TEXTURE_2D, buf_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Attach it to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           buf_texture, 0);
    // Add a renderbuffer object for depth and stencil testing
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // Set storage mode
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // Attach it
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    // Check success
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCheckFramebufferStatus.xhtml
    auto framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebuffer_status == GL_FRAMEBUFFER_COMPLETE) {
        ::Saturn::log("Framebuffer initialized");
    } else {
        // Fuck
        ::Saturn::error("Failed to create framebuffer");
        if (framebuffer_status == GL_FRAMEBUFFER_UNDEFINED) {
            ::Saturn::error("GL_FRAMEBUFFER_UNDEFINED");
        } else if (framebuffer_status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
            ::Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
            ::Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
            ::Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) {
            ::Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        } else if (framebuffer_status == GL_FRAMEBUFFER_UNSUPPORTED) {
            ::Saturn::error("GL_FRAMEBUFFER_UNSUPPORTED");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
            ::Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        } else if (framebuffer_status ==
                   GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) {
            ::Saturn::error("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        }
    }

    // Render to the screen again
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ::Saturn::log("Enabling OpenGL functionality");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::vector<glm::vec3> grass_positions(10);
    for (auto& pos : grass_positions) {
        // Generate random positions ranging from -2.5 to 2.5

        pos.x = (rand() % 100) / 10.0f - 5.0f;
        pos.y = 0.0f;
        pos.z = (rand() % 100) / 10.0f - 5.0f;
    }

    unsigned int grass_instance_vbo;
    glGenBuffers(1, &grass_instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, grass_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * grass_positions.size(),
                 &grass_positions[0], GL_STATIC_DRAW);
    glBindVertexArray(transparentVAO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // First render pass. Here we render everything to a buffer so we can do
        // some postprocessing later
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // default

        process_input(window);

        // Create model view projection matrices
        glm::mat4 model = identity(), view = identity(),
                  projection = identity();
        projection = glm::perspective(glm::radians(camera.Zoom),
                                      800.0f / 600.0f, 0.1f, 100.0f);
        // Remove translations so the
        // skybox doesn't move
        view = camera.view_matrix();
        //        view = glm::mat4(glm::mat3(view));

        // Sort grass_plants based on distance to avoid the depth test
        // discarding them
        std::sort(grass_plants.begin(), grass_plants.end(),
                  [](glm::vec3 const& a, glm::vec3 const& b) {
                      return glm::length(camera.Position - a) <
                             glm::length(camera.Position - b);
                  });
        // Reverse the list to draw objects further away first
        std::reverse(grass_plants.begin(), grass_plants.end());

        glUseProgram(mirror_shader);

        /*auto loc = glGetUniformLocation(shader, "texture1");
        assert(loc != -1);
        glUniform1i(loc, 0);*/

        glUniformMatrix4fv(glGetUniformLocation(mirror_shader, "view"), 1,
                           GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(mirror_shader, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));

        glBindTexture(GL_TEXTURE_CUBE_MAP,
                      cubemap_tex); // skybox to reflect in the mirror
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::translate(model, glm::vec3(2.0f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(270.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(glGetUniformLocation(mirror_shader, "model"), 1,
                           GL_FALSE, glm::value_ptr(model));
        //        nanosuit.render(mirror_shader);
        //        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Render a cube the 'easy' way
        model = identity();
        model = glm::translate(model, glm::vec3(2.0f, 3.0f, -5.0f));
        glUseProgram(shader); // Make sure the shader is active before rendering
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE,
                           glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
                           glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));
        cube_tex.bind();
        //        cube.render(shader);

        // Disable face culling for planes and quads
        glDisable(GL_CULL_FACE);
        glUseProgram(shader);
        floor.bind();
        model = identity();
        //        model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f),
                            glm::vec3(1.0f, 0.0f, 0.0f));

        glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE,
                           glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
                           glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));
        //        floor_plane.render(shader);

        glUseProgram(grass_shader);

        glBindVertexArray(transparentVAO);
        grass.bind();
        auto loc = glGetUniformLocation(grass_shader, "texture1");
        assert(loc != -1);
        glUniform1i(loc, 0);
        model = identity();
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(grass_shader, "view"), 1,
                           GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(grass_shader, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(window_shader, "model"), 1,
                           GL_FALSE, glm::value_ptr(model));
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, grass_positions.size());

        glBindVertexArray(0);

        // Render the skybox
        glDepthMask(0x00); // Disable writing to the depth buffer
        glUseProgram(skybox_shader);
        glUniformMatrix4fv(glGetUniformLocation(skybox_shader, "view"), 1,
                           GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skybox_shader, "projection"), 1,
                           GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex);
        //       glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(0xff); // re enable writing to the depth buffer

        // Second render pass. Render the buffer to the screen and use the
        // postprocessing shader
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Draw to window
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(framebuffer_shader);

        glBindVertexArray(screenVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, buf_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ::Saturn::log("Deallocating resources");

    glDeleteProgram(shader);
    glDeleteFramebuffers(1, &fbo);
    glDeleteVertexArrays(1, &transparentVAO);
    glDeleteVertexArrays(1, &screenVAO);
    glDeleteBuffers(1, &transparentVBO);
    glDeleteBuffers(1, &screenVBO);
    glDeleteTextures(1, &buf_texture);
    glDeleteRenderbuffers(1, &rbo);

    ::Saturn::log("Terminating");

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

} // namespace old
