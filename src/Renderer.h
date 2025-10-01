//
// Created by Hayden Chalin on 5/1/25.
//

#pragma once
#include <eigen/Eigen/Dense>
#include <MetalKit/MetalKit.hpp>
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
    void createDepthStencilState();
    void createDepthTexture(uint32_t w, uint32_t h);

    float getDeltaTime(){return deltaTime;};
    Window &getWindow();
    void drawFrame();


private:
    // Once
    MTL::Device *device{nullptr};
    Window *window{nullptr};
    MTL::CommandQueue *commandQueue{nullptr};
    MTL::RenderPipelineState *renderPipelineState{nullptr};
    MTL::DepthStencilState *depthStencilState{nullptr};
    MTL::Texture *depthTexture{nullptr};

    Eigen::Matrix4f projectionMatrix;          // ^ Camera space to clip space (screen)
    MTL::Buffer *uniformBuffer{nullptr};        // * For sending uniforms

    // Every frame
    MTL::CommandBuffer *commandBuffer{nullptr};

    // Model
    Model* model;

    Camera* camera;

    uint32_t width{0};
    uint32_t height{0};

    // Timeing
    double deltaTime {0.0};    // ^ Time between current and last frame
    double lastTime {0.0};
    double totalTime {0.0};
};



