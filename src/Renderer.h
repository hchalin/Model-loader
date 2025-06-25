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
#include "./Controller.h"

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
    // * Make this a controller class
    void cameraUp();
    void cameraDown();
    void cameraRight();
    void cameraLeft();
    void cameraZoom(float aZoom);
    void cameraMove(float aScalar);
    void cameraRotate(float aTurn);
    void processInput(int key, int scancode, int action, int mods);
    // Key map
    bool keyMap[GLFW_KEY_LAST + 1] = {false}; // Initialize all keys to false

private:
    // Once
    MTL::Device *device{nullptr};
    Window *window{nullptr};
    MTL::CommandQueue *commandQueue{nullptr};
    MTL::RenderPipelineState *renderPipelineState{nullptr};
    MTL::Buffer *triangleVertexBuffer{nullptr};
    MTL::Buffer *floorVertexBuffer{nullptr};
    MTL::Buffer *floorIndexBuffer{nullptr};

    Matrix4f projectionMatrix;          // ^ Camera space to clip space (screen)
    MTL::Buffer *uniformBuffer{nullptr};        // * For sending uniforms

    // Every frame
    MTL::CommandBuffer *commandBuffer{nullptr};

    // Camera
    Camera camera;

    // Controller
    Controller *controller;


    // Timeing
    float deltaTime {0.0};    // ^ Time between current and last frame
    float lastFrame {0.0};
};


/*
  *  Callback functions for GLFW. These must be free functions or static class methods.
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void framebuffer_refresh_callback(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
