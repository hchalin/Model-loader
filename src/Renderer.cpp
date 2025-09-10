//
// Created by Hayden Chalin on 5/1/25.
//

#include "Renderer.h"

using namespace Eigen;

Renderer::Renderer(MTL::Device * device, Window &windowSrc, Model *model, Camera *camera):
    // Get device from the metalLayer in the window
    device(device),
    window(&windowSrc),
    model(model),
    camera(camera)
{
    const float aRatio = windowSrc.getAspectRatio();
    if (!device) {
        throw std::runtime_error("Failed to create MTL::Device");
    }

    // ^ Timing
    lastTime = glfwGetTime();           // Set the initial time

    // ^ Command Queue
    constexpr int maxBufferAmt{64};
    commandQueue = device->newCommandQueue(maxBufferAmt);
    if (!commandQueue) {
        throw std::runtime_error("Failed to create command queue");
    }

    // ^ Allocate memory for the uniform buffer
    uniformBuffer = device->newBuffer(sizeof(Uniforms), MTL::ResourceStorageModeShared); // Room for33 matrices
    if (!uniformBuffer) {
        throw std::runtime_error("Failed to create uniform buffer");
    }


    // @ Rotate model Matrix
    BroMath::Transform &modelTransformMatrix = model->getTransformMatrix();
    modelTransformMatrix.setRotation(2.0f, 0.f, 1.f, 0.1f); // 0.4 rad ≈ 22.9°

    // Set the cameras viewMatrix in the uniform buffer
    auto* u = static_cast<Uniforms*>(uniformBuffer->contents());
    u->modelMatrix = modelTransformMatrix.getMatrix();
    u->viewProjectionMatrix = camera->getViewProjectionMatrix();
    // uniformBuffer->didModifyRange(NS::Range(0, sizeof(Uniforms)));          // ^ This is a complete flush


    // ^ Create render pipeline state
    createPipelineState();
}

void Renderer::createPipelineState() {
    /*
     * Shaders
     */
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
    } else {
        // @ If the library creation is successful, release the shader source
        if (shaderSource)
            shaderSource->release();
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
        vertexFunction->release();                                          // Release the vertex function
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

    // ^ This overwrites the entire color buffer, keep this in mind when rendering fog etc...
    colorAttachment->setBlendingEnabled(false);


    // ^ Describe the Vertex in memory
    MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

    // @ Attrubutes below
    // Position - [[ attribute(0) ]]
    vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(0)->setOffset(0);
    vertexDescriptor->attributes()->object(0)->setBufferIndex(0);
    // Color - [[ attribute(1) ]]
    vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(1)->setOffset(sizeof(float4));
    vertexDescriptor->attributes()->object(1)->setBufferIndex(0);
    // Normal - [[ attribute(2) ]]
    vertexDescriptor->attributes()->object(2)->setFormat(MTL::VertexFormatFloat3);
    vertexDescriptor->attributes()->object(2)->setOffset(offsetof(Vertex, normal));
    vertexDescriptor->attributes()->object(2)->setBufferIndex(0);
    // Texture Coord - [[ attribute(3) ]]
    vertexDescriptor->attributes()->object(3)->setFormat(MTL::VertexFormatFloat2);
    vertexDescriptor->attributes()->object(3)->setOffset(offsetof(Vertex, texCoord));
    vertexDescriptor->attributes()->object(3)->setBufferIndex(0);
    // Material - [[ attribute(4) ]]
    vertexDescriptor->attributes()->object(4)->setFormat(MTL::VertexFormatUInt);
    vertexDescriptor->attributes()->object(4)->setOffset(offsetof(Vertex, materialIndex));
    vertexDescriptor->attributes()->object(4)->setBufferIndex(0);


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
    if (commandQueue) {
        commandQueue->release();
        commandQueue = nullptr;
    }
    if (uniformBuffer) {
        uniformBuffer->release();
        uniformBuffer = nullptr;
    }
    if (renderPipelineState) {
        renderPipelineState->release();
        renderPipelineState = nullptr;
    }

}


void Renderer::drawFrame() {
/*
 *     This function draws a single frame
 *
 *      NOTE: Try to avoid per frame allocations
 */
    // Compute delta time here so both main loop and callbacks animate
    const double now = glfwGetTime();
    deltaTime = now - lastTime;
    lastTime = now;
    totalTime += deltaTime;

    // * Auto release pool for memory management
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
    // ^  Get the next drawable
    CA::MetalDrawable *drawable = window->getMTLLayer()->nextDrawable();        // Get the next drawable
    if (!drawable) {
        // * Clean up
        pool->release();
        glfwDestroyWindow(window->getGLFWWindow());
        return;
    }
    // Update per-frame uniforms BEFORE encoding:

    // Update per-frame camera VP in the uniform buffer
    {
        auto *u = static_cast<Uniforms *>(uniformBuffer->contents());
        u->viewProjectionMatrix = camera->getViewProjectionMatrix();
        // ^ Uniform buffer is declared as MTL::ResourceStorageModeShared - didModifyRange() is obsole
        // uniformBuffer->didModifyRange(NS::Range(offsetof(Uniforms, viewProjectionMatrix), sizeof(Eigen::Matrix4f)));
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
    colorAttachment->setClearColor(MTL::ClearColor(0.1, 0.1, 0.1, 1.0));
    colorAttachment->setStoreAction(MTL::StoreActionStore);

    // *  Encoding
    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
    encoder->setRenderPipelineState(renderPipelineState);
    encoder->setVertexBuffer(uniformBuffer, 0, 11);          // Set (write) the uniform buffer

    // cull mode
     encoder->setCullMode(MTL::CullMode::CullModeFront); // Culling the front works??



  // @ Draw model
  if (model) {
      model->getModelMatrix().setIdentity();
      // Bind buffers
      encoder->setVertexBuffer(model->getVertexBuffer(), 0, 0);
      encoder->setVertexBuffer(model->getMaterialBuffer(), 0, 1);        // Send the materials for the vertex fn in buffer 1
      encoder->setFragmentBuffer(model->getMaterialBuffer(), 0, 0);     // Send the materials for the fragment fn in buffer 0

      /*
       *      Rotation Scope
       */
       {
           float spinSpeed = 0.7f;
           BroMath::Transform &transformMatrix = model->getTransformMatrix();
           transformMatrix.setRotation(deltaTime * spinSpeed, 0.0f, 1.0f, 0.0f);
           auto *u = static_cast<Uniforms *>(uniformBuffer->contents());
           u->modelMatrix = transformMatrix.getMatrix();
       }


      // ^ This is the draw call
      encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
                                     model->getIndexCount(),
                                     MTL::IndexTypeUInt32,
                                     model->getIndexBuffer(),
                                     0,
                                     1);
    }


    encoder->endEncoding();

    // Present
    commandBuffer->presentDrawable(drawable);
    commandBuffer->commit();

    // ! CLEAN UP
    renderPassDescriptor->release();
    pool->release();
}


Window &Renderer::getWindow() {
      return *window;
}
