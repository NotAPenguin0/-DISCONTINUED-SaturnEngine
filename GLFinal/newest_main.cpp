#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "depend/stb_image.h"

#include "AbstractRenderer.hpp"
#include "Cube.hpp"
#include "FPSCamera.hpp"
#include "FPSCounter.hpp"
#include "FrameBuffer.hpp"
#include "FrameBufferRenderer.hpp"
#include "Input.hpp"
#include "InstancedMesh.hpp"
#include "InstancedRenderer.hpp"
#include "Model.hpp"
#include "OpenGL.hpp"
#include "Plane.hpp"
#include "PostProcessing.hpp"
#include "RenderTarget.hpp"
#include "ScreenRenderTarget.hpp"
#include "Shader.hpp"
#include "Skybox.hpp"
#include "SkyboxRenderer.hpp"
#include "Texture.hpp"
#include "TexturedObject.hpp"
#include "TexturedRenderer.hpp"
#include "Time.hpp"
#include "gl_errors.hpp"
#include "initgl.hpp"
#include "log.hpp"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

void register_keybinds(Saturn::Window& win) {
    using Saturn::Input;
    using Saturn::Time;

    Input::bind(GLFW_KEY_ESCAPE,
                [&win]() { glfwSetWindowShouldClose(win.handle(), true); });
    Input::bind(GLFW_KEY_W, []() {
        camera.move(Saturn::Direction::Forward, Time::deltaTime);
    });
    Input::bind(GLFW_KEY_S, []() {
        camera.move(Saturn::Direction::Backward, Time::deltaTime);
    });
    Input::bind(GLFW_KEY_A, []() {
        camera.move(Saturn::Direction::Left, Time::deltaTime);
    });
    Input::bind(GLFW_KEY_D, []() {
        camera.move(Saturn::Direction::Right, Time::deltaTime);
    });
    Input::bind(GLFW_KEY_SPACE,
                []() { camera.move(Saturn::Direction::Up, Time::deltaTime); });
    Input::bind(GLFW_KEY_LEFT_SHIFT, []() {
        camera.move(Saturn::Direction::Down, Time::deltaTime);
    });

    // Wireframe options
    Input::bind(GLFW_KEY_Z, []() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Saturn::info("Wireframe mode enabled");
    });
    Input::bind(GLFW_KEY_X, []() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        Saturn::info("Wireframe mode disabled");
    });

    glfwSetInputMode(win.handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

struct Matrices {
    glm::mat4 model = glm::mat4(1.0), view = glm::mat4(1.0),
              projection = glm::mat4(1.0);
};

void reset_mat(glm::mat4& mat) { mat = glm::mat4(1.0); }

void disable_movement(glm::mat4& view_matrix) {
    view_matrix = glm::mat4(glm::mat3(view_matrix));
}


int newest_main() {

    srand(time(nullptr));
    rand(); // might not always be random on some compilers

    Saturn::log("Starting initialization");
    auto main_window = init_all();

    main_window.set_position(550, 100);

    register_keybinds(main_window);

    enable_gl_option(GL_DEPTH_TEST);
    enable_gl_option(GL_CULL_FACE);
    enable_gl_option(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Saturn::Texture cube_tex;
    cube_tex.load("resources/textures/wood.png");
    Saturn::Texture floor_tex;
    floor_tex.load("resources/textures/wood_floor.png");
    Saturn::Texture grass_tex;
    grass_tex.load("resources/textures/grass.png", GL_TEXTURE0, GL_RGBA, true);

    Shader default_shader("resources/shaders/scene_tex_def_v.glsl",
                          "resources/shaders/scene_tex_def_f.glsl");
    Shader white_shader("resources/shaders/white_v.glsl",
                        "resources/shaders/white_f.glsl");

    Shader instanced_shader("resources/shaders/grass_v.glsl",
                            "resources/shaders/grass_f.glsl");
    Shader skybox_shader("resources/shaders/skybox_v.glsl",
                         "resources/shaders/skybox_f.glsl");
    Shader blinn_phong("resources/shaders/blinn_phong_v.glsl",
                       "resources/shaders/blinn_phong_f.glsl");
    Shader axis("resources/shaders/axis_v.glsl",
                "resources/shaders/axis_f.glsl");

    PostProcessingShaders postprocessing_shaders(
        "resources/shaders/postprocessing/postprocessing_effects.ppe");

    struct Meshes {
        Cube cube;
        Plane floor = Plane(1.0f);
        Plane grass;
    } meshes;

    // Todo: InstancedRenderer
    // Todo: Skybox
    // Todo: SkyboxRenderer

    TexturedObject textured_cube(meshes.cube, cube_tex);
    TexturedObject textured_floor(meshes.floor, floor_tex);
    TexturedObject textured_grass(meshes.grass, grass_tex);

    Skybox skybox("resources/skyboxes/forest.sb");

    std::vector<glm::vec3> grass_positions(10);
    for (auto& pos : grass_positions) {
        // Generate random positions ranging from -2.5 to 2.5

        pos.x = (rand() % 100) / 10.0f - 5.0f;
        pos.y = 0.0f;
        pos.z = (rand() % 100) / 10.0f - 5.0f;
    }

    InstancedMesh grass_instanced(textured_grass, grass_positions);

    AbstractRenderer renderer;
    TexturedRenderer texRenderer;
    FrameBufferRenderer framebufRenderer;

    ScreenRenderTarget screen;
    FrameBuffer framebuffer(
        glm::vec2(main_window.width(), main_window.height()));

    framebufRenderer.set_postprocessing_shader(
        postprocessing_shaders.get("none"));

    enable_gl_option(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, -8.0f);

    while (!glfwWindowShouldClose(main_window.handle())) {
        Saturn::Input::update(main_window);
        Saturn::Time::update();

        // First render pass
        framebuffer.clear(glm::vec3(0.1f, 0.1f, 0.1f),
                          GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        enable_gl_option(GL_CULL_FACE);
        enable_gl_option(GL_DEPTH_TEST);

        CHECK_GL_ERRORS();

        AbstractRenderer::RenderMatrices render_matrices;
        render_matrices.view = camera.view_matrix();
        render_matrices.projection = glm::perspective(
            glm::radians(camera.Zoom),
            (float)main_window.width() / main_window.height(), 0.1f,
            100.0f); // If you fly too high it will become visible that
                     // the far plane may be too close

        reset_mat(render_matrices.model);


        // Draw a small cube at the light position
        renderer.set_shader(white_shader);
        white_shader.use();
        render_matrices.model = glm::translate(render_matrices.model, lightPos);
        render_matrices.model =
            glm::scale(render_matrices.model, glm::vec3(0.2f, 0.2f, 0.2f));
        renderer.render_matrices = render_matrices;
        renderer.update_matrix_uniforms(AbstractRenderer::All);
        renderer.render(meshes.cube, framebuffer);

        texRenderer.set_shader(blinn_phong);
        blinn_phong.use();
        blinn_phong.set_vec3("lightPos", lightPos);
        blinn_phong.set_vec3("viewPos", camera.Position);

        reset_mat(render_matrices.model);
        render_matrices.model =
            glm::translate(render_matrices.model, glm::vec3(0.0f, -5.0f, 0.0f));
        render_matrices.model =
            glm::scale(render_matrices.model, glm::vec3(2.0f, 2.0f, 2.0f));
        render_matrices.model =
            glm::rotate(render_matrices.model, glm::radians(90.0f),
                        glm::vec3(1.0f, 0.0f, 0.0f));
        texRenderer.render_matrices = render_matrices;
        texRenderer.update_matrix_uniforms(AbstractRenderer::All);
        texRenderer.render(textured_floor, framebuffer);

        framebufRenderer.render(framebuffer, screen);

        CHECK_GL_ERRORS();

        glfwSwapBuffers(main_window.handle());
        glfwPollEvents();
    }

    main_window.destroy();
    glfwTerminate();

    return 0;
}
