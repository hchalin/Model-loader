// NOTE: KEEP THESE AT THE TOP!
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION

//------------------------------

#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include"Scene.h"


int main() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // * Create the window and renderer instances
    try {
        Scene scene;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    exit(EXIT_SUCCESS);
}