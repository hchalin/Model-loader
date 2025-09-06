//
// Created by Hayden Chalin on 5/1/25.
//

#include "Window.h"
#include "Renderer.h"

#include <iostream>
#include <ostream>
#include <Metal/MTLDevice.hpp>

Window::Window(MTL::Device *device) {
    // disable GLFW's default behavior of creating an OpenGL context
    // Look at declaration for better explination
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create window
    int windowWidth = 800, windowHeight = 600;
    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Model Viewer", nullptr, nullptr);
    if (!glfwWindow) {
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwSwapInterval(1);
    aspectRatio = (float)windowWidth / windowHeight;

    // Set the metal layer on the window
    metalLayer = CA::MetalLayer::layer()->retain();
    metalLayer->setDevice(device);    // Create metal device and set it as the metal layer for glfw
    metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);     // Standard 8-bit format.
    metalLayer->setFramebufferOnly(true);                       // Textures only as render targets


    nsWindow = get_ns_window(glfwWindow, metalLayer)->retain(); // Next Step window


     // glfwSetFramebufferSizeCallback(this->glfwWindow, framebuffer_size_callback);

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

// void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
//     auto *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
//     if (!renderer || height == 0) return;
//
//
//     renderer->getWindow().getMTLLayer()->setDrawableSize(CGSize{
//         static_cast<double>(width),
//         static_cast<double>(height)
//     });
//
//     float aspect = static_cast<float>(width) / height;
//     // renderer->updateProjectionMatrix(aspect);
//     // renderer->drawFrame();
// }