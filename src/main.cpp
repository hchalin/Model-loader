// NOTE: KEEP THESE AT THE TOP!
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
//------------------------------

#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "common/BroMath/vec4.h"
#include"Scene.h"
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.


#ifdef OLD
int main() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // * Create the window and renderer instances
    try {
        Window window;
        Renderer renderer = Renderer(window);
        renderer.render();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
#endif
int main() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // * Create the window and renderer instances
    try {
        Scene scene;
        scene.start();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}