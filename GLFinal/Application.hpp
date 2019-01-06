#ifndef SATURN_APPLICATION_HPP_
#define SATURN_APPLICATION_HPP_

#include "NonCopyable.hpp"

#include "FPSCamera.hpp"
#include "ObjectManager.hpp"
#include "Resource.hpp"
#include "ResourceLoaders.hpp"
#include "Window.hpp"
#include "World.hpp"

namespace Saturn {

// Only one instance of this class should be made. This is the class that
// controls the application (window, game loop, etc)
class Application : public NonCopyable {
public:
    Application(WindowSettings winsettings);
    ~Application();

    void init();
    void run();
    void quit();

    template<typename R>
    ResourceManager<R>& getResourceManager() {
        if constexpr (std::is_same_v<R, Resources::Model>)
            return modelManager;
        if constexpr (std::is_same_v<R, Resources::Texture>)
            return textureManager;
        if constexpr (std::is_same_v<R, Resources::Shader>)
            return shaderManager;
    }

private:
    static void mouse_callback([[maybe_unused]] GLFWwindow* win,
                               double xpos,
                               double ypos);
    static void scroll_callback([[maybe_unused]] GLFWwindow* window,
                                [[maybe_unused]] double xoffset,
                                double yoffset);

    Window window;
    World world;
    ResourceManager<Resources::Model> modelManager;
    ResourceManager<Resources::Texture> textureManager;
    ResourceManager<Resources::Shader> shaderManager;
    FPSCamera camera;

    ResourceRef<Resources::Shader> defaultShader;
    ResourceRef<Resources::Shader> texturedShader;
    ResourceRef<Resources::Shader> lightShader;

    // Testing
    int cubeId;
};

} // namespace Saturn

/*
-0.5f -0.5f -0.5f
0.5f -0.5f -0.5f
0.5f 0.5f -0.5f
 *0.5f 0.5f -0.5f
-0.5f 0.5f -0.5f
-0.5f -0.5f -0.5f
-0.5f -0.5f 0.5f
0.5f -0.5f 0.5f
0.5f 0.5f 0.5f
0.5f 0.5f 0.5f
-0.5f 0.5f 0.5f
-0.5f -0.5f 0.5f
-0.5f 0.5f 0.5f
-0.5f 0.5f -0.5f
-0.5f -0.5f -0.5f
-0.5f -0.5f -0.5f
-0.5f -0.5f 0.5f
-0.5f 0.5f 0.5f
0.5f 0.5f 0.5f
0.5f 0.5f -0.5f
0.5f -0.5f -0.5f
0.5f -0.5f -0.5f
0.5f -0.5f 0.5f
0.5f 0.5f 0.5f
-0.5f -0.5f -0.5f
0.5f -0.5f -0.5f
0.5f -0.5f 0.5f
0.5f -0.5f 0.5f
-0.5f -0.5f 0.5f
-0.5f -0.5f -0.5f
-0.5f 0.5f -0.5f
0.5f 0.5f -0.5f
0.5f 0.5f 0.5f
0.5f 0.5f 0.5f
-0.5f 0.5f 0.5f
-0.5f 0.5f -0.5f*/

#endif
