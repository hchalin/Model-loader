//
// Created by Hayden Chalin on 5/1/25.
//

#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "backend/glfw_adaptor.h"
#include <QuartzCore/CAMetalLayer.hpp>


class Camera;
class Renderer;

class Window {
    public:
    Window(MTL::Device *device);
    ~Window();

    CA::MetalLayer *getMTLLayer() const;
    GLFWwindow *getGLFWWindow() const;

    void setCamera(Camera *camera);
    void setRenderer(Renderer *renderer);

    Camera* camera;
    Renderer* renderer;

    float deltaTime(){return deltatime;};
    void setDeltaTime(float dt){deltatime = dt;};

    // Getters
    float getAspectRatio() const;

    float aspectRatio;
    void requestRedraw();


private:
    CA::MetalLayer* metalLayer{nullptr};
    GLFWwindow* glfwWindow{nullptr};
    NS::Window* nsWindow{nullptr};

    float deltatime{0.0};


    std::function<void(int, int)> resizeCallback;



};


