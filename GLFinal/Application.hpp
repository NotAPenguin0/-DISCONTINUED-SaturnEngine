#ifndef MVG_APPLICATION_HPP_
#define MVG_APPLICATION_HPP_

#include "NonCopyable.hpp"

#include "World.hpp"
#include "ObjectManager.hpp"
#include "Window.hpp"

namespace mvg {

// Only one instance of this class should be made. This is the class that
// controls the application (window, game loop, etc)
class Application : public NonCopyable {
public:
    Application(WindowSettings winsettings);
    ~Application();

    void init();
    void run();
    void quit();

private:
    Window window;
	World world;
};

} // namespace mvg

#endif
