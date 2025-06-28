//
// Created by Hayden Chalin on 6/23/25.
//

#include "Controller.h"

#include <GLFW/glfw3.h>



Controller::Controller(Camera &cam, Window * window):camera(cam), window(window) {

    std::cout << " CONTROLLER" << std::endl;
    // ^ Set GLFW Callbacks
    glfwSetWindowUserPointer(window->getGLFWWindow(), this);
}

Controller::~Controller() {
    return;
}

void Controller::handleInput(int key, int scancode, int mods) {
   std::cout << "Handle events" << std::endl;

}
void Controller::handleScroll(double xoffset, double yoffset) {
   std::cout << "Handle scroll" << std::endl;
}



void Controller::camUp() const{
    camera.moveUp(.05);
}

/**
 * @note Below are the free functions for glfw
 */
