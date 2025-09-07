//
// Created by Hayden Chalin on 9/6/25.
//

#include "Controller.h"

#include <iostream>
#include <ostream>

#include "GLFW/glfw3.h"

Controller::Controller() {
}

void Controller::pollEvents() {
    glfwPollEvents();
}

