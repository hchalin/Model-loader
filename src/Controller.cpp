//
// Created by Hayden Chalin on 6/23/25.
//

#include "Controller.h"

#include <GLFW/glfw3.h>



Controller::Controller(Camera &cam, Window * window):camera(cam), window(window) {

    std::cout << " CONTROLLER" << std::endl;
    // ^ Set GLFW Callbacks
    glfwSetWindowUserPointer(window->getGLFWWindow(), this);
    glfwSetKeyCallback(window->getGLFWWindow(), keyCallback);
    glfwSetScrollCallback(window->getGLFWWindow(), scrollCallback);
}

Controller::~Controller() {
    return;
}

void Controller::handleInput(int key, int action, int mods) {
    //glfwPollEvents();
   std::cout << "Handle events" << std::endl;
}
void Controller::handleScroll(double xoffset, double yoffset) {
   std::cout << "Handle scroll" << std::endl;
}
/**
 * @note Below are the free functions for glfw
 */


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS)
    {
    } else if (action == GLFW_RELEASE)
    {
    }
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    constexpr float dampen {0.09};      // Slow down trackpad movement
    constexpr float degreesPerScroll {5.0f};
    float angleDeg = xoffset * degreesPerScroll;
    //auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        std::cout << "Scroll: x = " << xoffset << ", y = " << yoffset << std::endl;
        std::cout << "ScrollCallback" << std::endl;
        //cameraRotate(-xoffset);
    }else {
    xoffset *= dampen;
    yoffset *= dampen;
    // TODO: Make this one function call?
    //cameraMove(xoffset);
    //cameraZoom(yoffset);
    }
};
