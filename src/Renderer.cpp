//
// Created by Hayden Chalin on 5/1/25.
//

#include "Renderer.h"

#include <iostream>
#include <ostream>

#include "Window.h"

Renderer::Renderer(Window window) {

    // Get device from the metalLayer in the window
    device = window.getMTLLayer()->device();
    if (!device) {
        throw std::runtime_error("Failed to create MTL::Device");
    }

    // Only one of these (right now)
    const int maxBufferAmt {64};
    commandQueue = device->newCommandQueue(maxBufferAmt);
    if (!commandQueue) {
        throw std::runtime_error("Failed to create command queue");
    }

}

void Renderer::createPipelineState() {
    MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

    MTL::Library *library = device->newDefaultLibrary();
    if (!library) {
        throw std::runtime_error("Failed to create MTL::Library");
    }
    const std::string shaderFileName = "shaders.metal";
    NS::Error * err {nullptr};

    // Load shader library
    try {
        loadShaderFromFile(library, device, shaderFileName);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    // Create shader functions
    try {
        MTL::Function *vertexFunction = library->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
        MTL::Function *fragmentFunction = library->newFunction(
            NS::String::string("fragment_main", NS::UTF8StringEncoding));
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }




    //renderPipelineState = device->newRenderPipelineState();
}

Renderer::~Renderer() {

    // Do not handle deleting device here
    device = nullptr;

    commandQueue->release();
}

