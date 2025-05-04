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
    glfwWindow = glfwCreateWindow(800, 600, "Model Viewer", nullptr, nullptr);
    if (!glfwWindow) {
        throw std::runtime_error("Failed to create GLFW window");
    }

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
