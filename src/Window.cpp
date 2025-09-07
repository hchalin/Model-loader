//
// Created by Hayden Chalin on 5/1/25.
//

#include "Window.h"
#include "Renderer.h"

#include <iostream>
#include <ostream>
#include <Metal/MTLDevice.hpp>
// Forward-declare static callbacks at file scope
static void framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height);
static void framebuffer_refresh_callback(GLFWwindow* glfwWindow);

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


    // ^ Set GLFW Callbacks
    glfwSetWindowUserPointer(this->glfwWindow, this);
    glfwSetWindowRefreshCallback(this->glfwWindow, framebuffer_refresh_callback);       // ! Not in use
    glfwSetFramebufferSizeCallback(this->glfwWindow, framebuffer_size_callback);        // @ On Resize

}

Window::~Window() {
    if (metalLayer) {
        metalLayer->release();
    }
}

void Window::setCamera(Camera *camera) {
    this->camera = camera;
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

void framebuffer_refresh_callback(GLFWwindow *glfwWindow) {
    std::cout << "Refresh" << std::endl;
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window) return;
    // Draw a frame while the OS runs the modal refresh cycle
    window->requestRedraw();

}

void Window::setRenderer(Renderer *renderer) {
    if (renderer) {
        this->renderer = renderer;
    }
    if (renderer)
        std::cout << "Renderer set" << std::endl;
}





void Window::requestRedraw() {
    std::cout << "DeltaTime: " << this->deltaTime() << std::endl;
    if (renderer)
        renderer->drawFrame(this->deltaTime());
}

static void framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height) {
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window || height == 0) return;

    window->getMTLLayer()->setDrawableSize(CGSize{
        static_cast<double>(width),
        static_cast<double>(height)
    });

    float aspect = static_cast<float>(width) / height;
    window->aspectRatio = aspect; // keep cached aspect in sync
    if (window->camera) {
        window->camera->updateAspectRatio(aspect);
    }
    window->requestRedraw();
}