//
// Created by Hayden Chalin on 5/1/25.
//

#include "Window.h"

#include <iostream>
#include <ostream>
#include <Metal/MTLDevice.hpp>

Window::Window() {
    // disable GLFW's default behavior of creating an OpenGL context
    // Look at declaration for better explination
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create window
    int windowWidth = 800, windowHeight = 600;
    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Model Viewer", nullptr, nullptr);
    if (!glfwWindow) {
        throw std::runtime_error("Failed to create GLFW window");
    }
    aspectRatio = (float)windowWidth / windowHeight;

    // Set the metal layer on the window
    metalLayer = CA::MetalLayer::layer()->retain();
    metalLayer->setDevice(MTL::CreateSystemDefaultDevice());
    metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);     // Standard 8-bit format.
    metalLayer->setFramebufferOnly(true);                       // Textures only as render targets

    nsWindow = get_ns_window(glfwWindow, metalLayer)->retain();
}

Window::~Window() {
    if (metalLayer) {
        metalLayer->release();
    }
}

CA::MetalLayer *Window::getMTLLayer() const {
    if (metalLayer) {
        return metalLayer;
    }
    return nullptr;
}
GLFWwindow *Window::getGLFWWindow() const {
    if (metalLayer) {
        return glfwWindow;
    }
    return nullptr;
}

float Window::getAspectRatio() const {
    return aspectRatio;
}
/*
void Window::setResizeCallback(std::function<void(int, int)> callback) {
    resizeCallback = callback;

    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
        Window* windowObj = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (windowObj && windowObj->resizeCallback) {
            // Update Metal layer drawable size
            if (windowObj->metalLayer) {
                windowObj->metalLayer->setDrawableSize(CGSize{static_cast<double>(width),
                                                              static_cast<double>(height)});
            }

            windowObj->resizeCallback(width, height);
            windowObj->aspectRatio = static_cast<float>(width) / height;
        }
    });
}
*/

std::pair<int, int> Window::getSize() const {
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);
    return {width, height};
}