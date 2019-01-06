#include "KeepOpen.hpp"

#include "Application.hpp"
#include "ComponentSystem.hpp"
#include "Components.hpp"
#include "Object.hpp"
#include "World.hpp"

using namespace Saturn;
using namespace Components;

// Old main function, this runs the application we want to get eventually, but
// in a cleaner and more flexible way
int main2();

int newest_main();

// #TODO:
// -Fix specular lighting maps
// -Lights are assigned to objects!! WRONG!!! HAS TO BE INDEPENDANT!!

int main() {

    newest_main();

    KeepOpen k; // Keep the console open on windows
    //    Application app(WindowSettings("Saturn Engine", 800, 600));
    // Run the application. This enters the main loop, and blocks the current
    // thread until the application quits
    //    app.run();

    return 0;
}
