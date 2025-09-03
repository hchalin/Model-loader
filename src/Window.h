//
// Created by Hayden Chalin on 5/1/25.
//

#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "backend/glfw_adaptor.h"
#include <QuartzCore/CAMetalLayer.hpp>
class Window {
    public:
    Window(MTL::Device *device);
    ~Window();

    CA::MetalLayer *getMTLLayer() const;
    GLFWwindow *getGLFWWindow() const;

    // Getters
    float getAspectRatio() const;


private:
    CA::MetalLayer* metalLayer{nullptr};
    GLFWwindow* glfwWindow{nullptr};
    NS::Window* nsWindow{nullptr};

    float aspectRatio;

    std::function<void(int, int)> resizeCallback;


};


