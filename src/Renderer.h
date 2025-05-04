//
// Created by Hayden Chalin on 5/1/25.
//

#pragma once
#include <Metal/Metal.hpp>

#include "Window.h"
#include "shaders/readShaderFile.h"


class Renderer {
public:
    Renderer(Window window);
    ~Renderer();

    void createPipelineState();


private:
    // Once
    MTL::Device *device{nullptr};
    MTL::CommandQueue *commandQueue{nullptr};
    MTL::RenderPipelineState *renderPipelineState{nullptr};

    // Every frame
    MTL::CommandBuffer *commandBuffer{nullptr};
};



