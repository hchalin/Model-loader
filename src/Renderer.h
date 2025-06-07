//
// Created by Hayden Chalin on 5/1/25.
//

#pragma once
#include <Metal/Metal.hpp>
#include <eigen/Eigen/Dense>

#include <iostream>
#include <ostream>

#include "Window.h"
#include "common/BroMath/Transform.h"
#include "common/common.h"
#include "shaders/readShaderFile.h"
#include "./Camera.h"

// Used for offset
struct Uniforms {
    Matrix4f viewMatrix;
    Matrix4f projectionMatrix;
};
class Renderer {
public:
    Renderer();
    Renderer(Window &window);
    ~Renderer();

    void createPipelineState();
    void render();

    void updateProjectionMatrix(float aRatio);

    Window &getWindow();
    void drawFrame();

    // Camera controls
    void cameraUp();
    void cameraDown();
    void cameraRight();
    void cameraLeft();
    void cameraZoom(float aZoom);
    void cameraTurn(float aTurn);


private:
    // Once
    MTL::Device *device{nullptr};
    Window *window{nullptr};
    MTL::CommandQueue *commandQueue{nullptr};
    MTL::RenderPipelineState *renderPipelineState{nullptr};
    MTL::Buffer *vertexBuffer{nullptr};

    Matrix4f projectionMatrix;          // ^ Camera space to clip space (screen)
    MTL::Buffer *uniformBuffer{nullptr};        // * For sending uniforms

    // Every frame
    MTL::CommandBuffer *commandBuffer{nullptr};

    // Camera
    Camera camera;
};


/*
 *  Callback functions for glfw. These have to be free static functions.
*/
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void framebuffer_refresh_callback(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
