//
// Created by Hayden Chalin on 5/1/25.
//

#include "Renderer.h"

#include <iostream>
#include <ostream>

#include "Window.h"

Renderer::Renderer(Window windowSrc) {

    // Get device from the metalLayer in the window
    device = windowSrc.getMTLLayer()->device();
    if (!device) {
        throw std::runtime_error("Failed to create MTL::Device");
    }

   this->window = &windowSrc;


    // Only one of these (right now)
    constexpr int maxBufferAmt {64};
    commandQueue = device->newCommandQueue(maxBufferAmt);
    if (!commandQueue) {
        throw std::runtime_error("Failed to create command queue");
    }

    // Create render pipeline state
    createPipelineState();

    render();
}

void Renderer::createPipelineState() {
    MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

    const std::string shaderFileName = "shaders.metal";
    NS::Error * err {nullptr};
    MTL::Library *library {nullptr};
    NS::String *shaderSource {nullptr};

    try {
        shaderSource = loadShaderFromFile(device, shaderFileName);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    NS::Error *error = nullptr;
    MTL::CompileOptions *compileOptions = MTL::CompileOptions::alloc()->init();
    library = device->newLibrary(shaderSource, compileOptions, &error);
    if (!library) {
        compileOptions->release();
        throw std::runtime_error("Failed to create MTL::Library");
    }
    // Create shader functions
    MTL::Function *vertexFunction {nullptr};
    MTL::Function *fragmentFunction {nullptr};
    // After:
    vertexFunction = library->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
    if (!vertexFunction) {
        std::cerr << "Failed to create vertex function" << std::endl;
        library->release();
        throw std::runtime_error("Failed to create vertex function");
    }

    fragmentFunction = library->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
    if (!fragmentFunction) {
        std::cerr << "Failed to create fragment function" << std::endl;
        vertexFunction->release();
        library->release();
        throw std::runtime_error("Failed to create fragment function");
    }


    // Create pipeline descriptor and set functions
    MTL::RenderPipelineDescriptor *renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    renderPipelineDescriptor->setVertexFunction(vertexFunction);
    renderPipelineDescriptor->setFragmentFunction(fragmentFunction);

    // Configure color attachment - NOTE: The color attachment represents the output target for the fragment shader.
    MTL::RenderPipelineColorAttachmentDescriptor *colorAttachment = renderPipelineDescriptor->colorAttachments()->object(0);
    colorAttachment->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
    colorAttachment->setBlendingEnabled(false);
    // This overwrites the entire color buffer, keep this in mind when rendering fog etc...

    renderPipelineState = device->newRenderPipelineState(renderPipelineDescriptor, &err);
    if (!renderPipelineState) {
        throw std::runtime_error("Failed to create MTL::RenderPipelineState");
    }
    else {
        std::cout << "MTL::RenderPipelineState created!" << std::endl;
    }

    // Resource clean up
    library->release();
    compileOptions->release();
    vertexFunction->release();
    fragmentFunction->release();
    renderPipelineDescriptor->release();

}

Renderer::~Renderer() {

    // Do not handle deleting device here
    device = nullptr;

    commandQueue->release();
}

/*
void Renderer::render() {
    // Get drawable
    CA::MetalDrawable *drawable {nullptr};
    try {
       drawable = window->getMTLLayer()->nextDrawable();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    if (!drawable) {
        throw std::runtime_error("Failed to render drawable");
    }

    // Create command buffer
    MTL::CommandBuffer *commandBuffeer = commandQueue->commandBuffer();
    if (!commandBuffeer) {
        throw std::runtime_error("Failed to render command buffer");
    }


}
*/
void Renderer::render() {
    while (!glfwWindowShouldClose(window->getGLFWWindow())) {
        glfwPollEvents();
        NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
        CA::MetalDrawable *drawable = window->getMTLLayer()->nextDrawable();
        if (!drawable) {
            // * Clean up
            pool->release();
            glfwDestroyWindow(window->getGLFWWindow());
            break;
        }

        // * Create command buffer
        MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
        if (!commandBuffer) {
            throw std::runtime_error("Failed to render command buffer");
        }

        // * Create render pass descriptor
        MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
        MTL::RenderPassColorAttachmentDescriptor *colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
        colorAttachment->setTexture(drawable->texture());
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setClearColor(MTL::ClearColor(1.0, 1.0, 1.0, 1.0));
        colorAttachment->setStoreAction(MTL::StoreActionStore);

        // *      Encoding
        MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);

        /*
         * Triangle
         */



        encoder->endEncoding();

        // Present
        commandBuffer->presentDrawable(drawable);
        commandBuffer->commit();

        // ! CLEAN UP
        renderPassDescriptor->release();
        pool->release();
    }
}


