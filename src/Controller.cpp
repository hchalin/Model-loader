//
// Created by Hayden Chalin on 9/6/25.
//

#include "Controller.h"

#include <iostream>
#include <ostream>

#include "GLFW/glfw3.h"

Controller::Controller() {

    // glfwSetKeyCallback(window->getGLFWWindow(), keyCallback);
    // glfwSetScrollCallback(window->getGLFWWindow(), scrollCallback);
}

Controller::~Controller(){}

void Controller::pollEvents(Camera * camera) {
    glfwPollEvents();
}

