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

    metalLayer = CA::MetalLayer::layer()->retain();
    //metalLayer->setDevice(MTL::CreateSystemDefaultDevice());
}

Window::~Window(){}