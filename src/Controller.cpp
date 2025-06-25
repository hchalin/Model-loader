//
// Created by Hayden Chalin on 6/23/25.
//

#include "Controller.h"

#include <GLFW/glfw3.h>

#include "Renderer.h"


Controller::Controller(Camera &cam):camera(cam) {

    // ^ Set GLFW Callbacks
    glfwSetWindowUserPointer(window->getGLFWWindow(), this);
    glfwSetWindowRefreshCallback(window->getGLFWWindow(), framebuffer_refresh_callback);
    glfwSetKeyCallback(window->getGLFWWindow(), keyCallback);
    glfwSetScrollCallback(window->getGLFWWindow(), scrollCallback);
}

Controller::~Controller() {
    return;
}

void Controller::handleEvents() {
    //glfwPollEvents();
    //std::cout << "Handle events" << std::endl;
}
/**
 * @note Below are the free functions for glfw
 */

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    auto *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
    if (!renderer || height == 0) return;


    renderer->getWindow().getMTLLayer()->setDrawableSize(CGSize{
        static_cast<double>(width),
        static_cast<double>(height)
    });

    float aspect = static_cast<float>(width) / height;
    renderer->updateProjectionMatrix(aspect);
    renderer->drawFrame();
}


void framebuffer_refresh_callback(GLFWwindow* window) {
    // ! Not currently in use
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS)
    {
        renderer->keyMap[key] = true;
    } else if (action == GLFW_RELEASE)
    {
        renderer->keyMap[key] = false;
    }
    renderer->processInput(key, scancode, action, mods);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    constexpr float dampen {0.09};      // Slow down trackpad movement
    constexpr float degreesPerScroll {5.0f};
    float angleDeg = xoffset * degreesPerScroll;
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        std::cout << "Scroll: x = " << xoffset << ", y = " << yoffset << std::endl;
        std::cout << "ScrollCallback" << std::endl;
        renderer->cameraRotate(-xoffset);
    }else {
    xoffset *= dampen;
    yoffset *= dampen;
    // TODO: Make this one function call?
    renderer->cameraMove(xoffset);
    renderer->cameraZoom(yoffset);
    }
};
