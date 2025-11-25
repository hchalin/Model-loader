//
// Created by Hayden Chalin on 9/6/25.
//

#include "Controller.h"

#include "Window.h"
#include <iostream>
#include <ostream>

#include "Camera.h"
#include "common/BroMath/Transform.h"
#include "GLFW/glfw3.h"

Controller::Controller(Camera * cam, Window * win):camera(cam), window(win) {

    // glfwSetKeyCallback(window->getGLFWWindow(), keyCallback);
    // glfwSetScrollCallback(window->getGLFWWindow(), scrollCallback);
    if (!camera && window)
        throw std::runtime_error("Camera and Window must be set");
    // glfwSetMouseButtonCallback(window->getGLFWWindow(), mouseButtonCallback)
    if (window) {
        glfwSetKeyCallback(window->getGLFWWindow(), keyCallback);
        glfwSetScrollCallback(window->getGLFWWindow(), scrollCallback);
    } else {
        throw std::runtime_error(std::string("Window must be set to inizialize Controller ")
                                 + std::string(__FILE__) + ":" + std::to_string(__LINE__));
    }
}

Controller::~Controller(){}

void Controller::pollEvents() {
    glfwPollEvents();
}

void Controller::processKey(int key, int scancode, int action, int mods) {
    // Only handle one-shot actions here (e.g., ESC). Movement is handled in update(dt).
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window->getGLFWWindow(), GL_TRUE);
    }
}

// Smooth per-frame movement using current key states and dt
void Controller::update(float dt) {
    /*
     *      Change controller mapping here
     *
     */
    if (!camera || !window) return;

    // Continuous input -> movement
    if (keyMap[GLFW_KEY_A]) camera->moveLeft(dt);
    if (keyMap[GLFW_KEY_D]) camera->moveRight(dt);
    if (keyMap[GLFW_KEY_S]) camera->moveDown(dt);
    if (keyMap[GLFW_KEY_W]) camera->moveUp(dt);
    //if (keyMap[GLFW_KEY_LEFT_CONTROL] && keyMap[GLFW_KEY_W]) {
       //std::cout << "CONTROL PRESSED" << std::endl;
        //camera->moveUp(dt);
    //}
}

// * glfw free functions
void keyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

    if (action == GLFW_PRESS)
    {
        window->controller->keyMap[key] = true;
    } else if (action == GLFW_RELEASE)
    {
        window->controller->keyMap[key] = false;
    }
    if (window->controller)
     window->controller->processKey(key, scancode, action, mods);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    std::cout << "Scroll" << std::endl;
}

