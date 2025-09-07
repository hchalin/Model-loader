//
// Created by Hayden Chalin on 5/1/25.
//

#pragma once
#include <eigen/Eigen/Dense>
#include <Metal/Metal.hpp>

#include <iostream>
#include <ostream>

#include "Window.h"
#include "common/BroMath/Transform.h"
#include "common/common.h"
#include "shaders/readShaderFile.h"
#include "./Camera.h"
#include "Model.h"

// ^ Used for offset, make sure this matches the Uniforms struct in shaders.metal at all times
struct Uniforms {
    Eigen::Matrix4f viewMatrix;
    Eigen::Matrix4f projectionMatrix;
    Eigen::Matrix4f modelMatrix;
    Eigen::Matrix4f viewProjectionMatrix;      // This is the matrix multiple of view and proj
};

class Renderer {
public:
    Renderer();
    Renderer(MTL::Device * device, Window &window, Model * model, Camera *camera);
    ~Renderer();

    void createPipelineState();
    void render(Eigen::Matrix4f &viewMatrix, Eigen::Matrix4f &projectionMatrix, Eigen::Matrix4f &modelMatrix);
    void render(Camera * cam, Model * model);


    Window &getWindow();
    void drawFrame(double dT);


private:
    // Once
    MTL::Device *device{nullptr};
    Window *window{nullptr};
    MTL::CommandQueue *commandQueue{nullptr};
    MTL::RenderPipelineState *renderPipelineState{nullptr};
    MTL::Buffer *triangleVertexBuffer{nullptr};
    MTL::Buffer* modelBuffer{nullptr};
    MTL::Buffer *floorVertexBuffer{nullptr};
    MTL::Buffer *floorIndexBuffer{nullptr};

    Eigen::Matrix4f projectionMatrix;          // ^ Camera space to clip space (screen)
    MTL::Buffer *uniformBuffer{nullptr};        // * For sending uniforms

    // Every frame
    MTL::CommandBuffer *commandBuffer{nullptr};


    // Model
    Model* model;

    Camera* camera;

    // Timeing
    double deltaTime {0.0};    // ^ Time between current and last frame
    double lastTime {0.0};
    double totalTime {0.0};
};


/*
  *  Callback functions for GLFW. These must be free functions or static class methods.
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void framebuffer_refresh_callback(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
