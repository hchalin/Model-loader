//
// Created by Hayden Chalin on 5/1/25.
//

#include "Renderer.h"

#include <iostream>
#include <ostream>


Renderer::Renderer(Window &windowSrc):
    // Get device from the metalLayer in the window
    device(windowSrc.getMTLLayer()->device()),
    window(&windowSrc),
    camera(Vector3f(0.0, 0.0, 3.0), Vector3f(0.0, 0.0, 0.0)) // * camera(camPos, target)
{
    const float aRatio = windowSrc.getAspectRatio();
    if (!device) {
        throw std::runtime_error("Failed to create MTL::Device");
    }

    // ^ Command Queue
    constexpr int maxBufferAmt{64};
    commandQueue = device->newCommandQueue(maxBufferAmt);
    if (!commandQueue) {
        throw std::runtime_error("Failed to create command queue");
    }

    // ^ Allocate memory for the uniform buffer
    uniformBuffer = device->newBuffer(sizeof(Matrix4f)* 2, MTL::ResourceStorageModeManaged);    // Room for 2 matrices
    if (!uniformBuffer) {
        throw std::runtime_error("Failed to create uniform buffer");
    }
    updateProjectionMatrix(aRatio);

    // Add in Renderer constructor after creating uniformBuffer
    window->setResizeCallback([this](int width, int height) {
        float newAspectRatio = static_cast<float>(width) / static_cast<float>(height);
        updateProjectionMatrix(newAspectRatio);
    });

    // ^ Camera view matrix
    Matrix4f viewMatrix = camera.getViewMatrix();
    Matrix4f *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents()); // @ Get pointer to buffers contents
    // @ *bufferPtr = viewMatrix uses the Matrix4f assignment operator to copy all the data
    *bufferPtr = viewMatrix; // @ This will do something similar to memcopy into the uniform buffer
    // ! bufferPtr = &viewMatrix;   This does not work, this just updates the pointers address
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));


    //std::cout << "View matrix: \n" << viewMatrix << std::endl;

    // ^ Create render pipeline state
    createPipelineState();

    render();
}

void Renderer::updateProjectionMatrix(const float aRatio) {

    // ^ Define the vertical fov to radias, horizontal will be based off of this.
    const float fovY = 45.0f * (M_PI / 180.0f);

    // ^ Frustrum clipping planes
    const float near = 0.1f;
    const float far = 100.0f;

    // ^ Reset projectionMatrix
    projectionMatrix.setIdentity();

    float tanHalfFovy = std::tan(fovY / 2.0f);

    // Set up perspective matrix (column-major order in Eigen)
    projectionMatrix(0,0) = 1.0f / (aRatio * tanHalfFovy); // Scale X
    projectionMatrix(1,1) = 1.0f / tanHalfFovy;            // Scale Y
    projectionMatrix(2,2) = -(far + near) / (far - near);  // Scale and translate Z
    projectionMatrix(2,3) = -(2.0f * far * near) / (far - near); // Perspective divide term
    projectionMatrix(3,2) = -1.0f;                         // Enables perspective division
    projectionMatrix(3,3) = 0.0f;                          // Required for perspective

    // Combine with view matrix and update uniform buffer
    //Matrix4f viewProjection = projectionMatrix * camera.getViewMatrix();

    // Update uniform buffer with combined view-projection matrix
    Matrix4f* bufferPtr = static_cast<Matrix4f*>(uniformBuffer->contents());
    *(bufferPtr + 1) = projectionMatrix;
    uniformBuffer->didModifyRange(NS::Range(sizeof(Matrix4f), sizeof(Matrix4f)));

    //std::cout << "Projection matrix:\n" << projectionMatrix << std::endl;

}

void Renderer::createPipelineState() {
    MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

    const std::string shaderFileName = "shaders.metal";
    NS::Error *err{nullptr};
    MTL::Library *library{nullptr};
    NS::String *shaderSource{nullptr};

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
    // ^ Create shader functions
    MTL::Function *vertexFunction{nullptr};
    MTL::Function *fragmentFunction{nullptr};

    // ^ Vertex Function
    vertexFunction = library->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
    if (!vertexFunction) {
        std::cerr << "Failed to create vertex function" << std::endl;
        library->release();
        throw std::runtime_error("Failed to create vertex function");
    }

    // ^ Fragment Function
    fragmentFunction = library->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
    if (!fragmentFunction) {
        std::cerr << "Failed to create fragment function" << std::endl;
        vertexFunction->release();
        library->release();
        throw std::runtime_error("Failed to create fragment function");
    }


    // ^ Create pipeline descriptor and set functions
    MTL::RenderPipelineDescriptor *renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    renderPipelineDescriptor->setVertexFunction(vertexFunction);
    renderPipelineDescriptor->setFragmentFunction(fragmentFunction);

    // ^ Configure color attachment - NOTE: The color attachment represents the output target for the fragment shader.
    MTL::RenderPipelineColorAttachmentDescriptor *colorAttachment = renderPipelineDescriptor->colorAttachments()->
            object(0);
    colorAttachment->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
    colorAttachment->setBlendingEnabled(false);
    // This overwrites the entire color buffer, keep this in mind when rendering fog etc...

    /*
     * Triangle
     */

    Vertex vertices[] = {
        {{0.0, 0.5, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}}, // Top (red)
        {{-0.5, -0.5, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}}, // Bottom left (green)
        {{0.5, -0.5, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0}} // Bottom right (blue)
    };
    vertexBuffer = device->newBuffer(vertices, sizeof(vertices), MTL::ResourceStorageModeManaged);
    if (!vertexBuffer) {
        throw std::runtime_error("Failed to create vertex buffer");
    }

    MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

    // Position
    vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(0)->setOffset(0);
    vertexDescriptor->attributes()->object(0)->setBufferIndex(0);
    // Color
    vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(1)->setOffset(sizeof(float4));
    vertexDescriptor->attributes()->object(1)->setBufferIndex(0);
    //Layout
    vertexDescriptor->layouts()->object(0)->setStride(sizeof(Vertex));

    renderPipelineDescriptor->setVertexDescriptor(vertexDescriptor);

    renderPipelineState = device->newRenderPipelineState(renderPipelineDescriptor, &err);
    if (!renderPipelineState) {
        throw std::runtime_error("Failed to create MTL::RenderPipelineState");
    }

    // Resource clean up
    library->release();
    compileOptions->release();
    vertexFunction->release();
    fragmentFunction->release();
    renderPipelineDescriptor->release();
    vertexDescriptor->release();
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
    auto [lastWidth, lastHeight] = window->getSize();

    while (!glfwWindowShouldClose(window->getGLFWWindow())) {
        glfwPollEvents();

        // ^ Check for window size changes every frame
        auto [width, height] = window->getSize();
        // ! This freezes when I click and drag a corner?
        std::cout << "resizing" << std::endl;
        std::cout << "width: " << width << std::endl;
        std::cout << "height: " << height << std::endl;
        if (width != lastWidth || height != lastHeight)
        {
            float newAspectRatio = static_cast<float>(width) / static_cast<float>(height);
            updateProjectionMatrix(newAspectRatio);
            lastWidth = width;
            lastHeight = height;
        }

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

        encoder->setRenderPipelineState(renderPipelineState);
        encoder->setVertexBuffer(vertexBuffer, 0, 0);
        encoder->setVertexBuffer(uniformBuffer, 0, 1);
        encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

        encoder->endEncoding();

        // Present
        commandBuffer->presentDrawable(drawable);
        commandBuffer->commit();

        // ! CLEAN UP
        renderPassDescriptor->release();
        pool->release();
    }
}
