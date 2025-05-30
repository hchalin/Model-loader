//
// Created by Hayden Chalin on 5/1/25.
//

#pragma once
#include <Metal/Metal.hpp>
#include <eigen/Eigen/Dense>

#include "Window.h"
#include "common/BroMath/Transform.h"
#include "common/common.h"
#include "shaders/readShaderFile.h"
#include "./Camera.h"


class Renderer {
public:
    Renderer();
    Renderer(Window &window);
    ~Renderer();

    void createPipelineState();
    void render();


private:
    // Once
    MTL::Device *device{nullptr};
    Window *window{nullptr};
    MTL::CommandQueue *commandQueue{nullptr};
    MTL::RenderPipelineState *renderPipelineState{nullptr};
    MTL::Buffer *vertexBuffer{nullptr};

    Matrix4f projectionMatrix;
    MTL::Buffer *uniformBuffer{nullptr};
    void updateProjectionMatrix(const float aRatio);

    // Every frame
    MTL::CommandBuffer *commandBuffer{nullptr};

    // Camera
    Camera camera;
};



