//
// Created by Hayden Chalin on 5/1/25.
//

#include "Renderer.h"



Renderer::Renderer(Window &windowSrc):
    // Get device from the metalLayer in the window
    device(windowSrc.getMTLLayer()->device()),
    window(&windowSrc),
    camera(Vector3f(1.0, 0.0, 5.0), Vector3f(0.0, 0.0, 0.0)) // * camera(camPos, target)
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


    // ^ Camera view matrix
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents()); // @ Get pointer to buffers contents
    // @ *bufferPtr = viewMatrix uses the Matrix4f assignment operator to copy all the data
    *bufferPtr = viewMatrix; // @ This will do something similar to memcopy into the uniform buffer
    // ! bufferPtr = &viewMatrix;   This does not work, this just updates the pointers address
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));

    // ^ Set GLFW Callbacks
    glfwSetWindowUserPointer(window->getGLFWWindow(), this);
    glfwSetWindowRefreshCallback(window->getGLFWWindow(), framebuffer_refresh_callback);
    glfwSetFramebufferSizeCallback(window->getGLFWWindow(), framebuffer_size_callback);
    glfwSetKeyCallback(window->getGLFWWindow(), keyCallback);
    glfwSetScrollCallback(window->getGLFWWindow(), scrollCallback);

    // ^ Create render pipeline state
    createPipelineState();

}

void Renderer::updateProjectionMatrix(float aRatio) {


    std::cout << "aspect Ratio: " << aRatio << std::endl;

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

    // Update uniform buffer with combined view-projection matrix
    auto* bufferPtr = static_cast<Matrix4f*>(uniformBuffer->contents());
    *(bufferPtr + 1) = projectionMatrix;
    uniformBuffer->didModifyRange(NS::Range(offsetof(Uniforms, projectionMatrix), sizeof(Matrix4f)));

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

    // ^ This overwrites the entire color buffer, keep this in mind when rendering fog etc...
    colorAttachment->setBlendingEnabled(false);

    /*
     * Triangle
     */
    Vertex vertices[] = {
        {{0.0, 0.5, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}}, // Top (red)
        {{-0.5, -0.5, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}}, // Bottom left (green)
        {{0.5, -0.5, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0}} // Bottom right (blue)
    };

    // ^ Create vertex buffer
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

void Renderer::render() {
    // ^ https://stackoverflow.com/questions/23858454/glfw-poll-waits-until-window-resize-finished-how-to-fix
    // ! FIXME: Window freezes on resize, multi-theading is not a solution per GLFW's docs, GLFW is not thread safe.

    while (!glfwWindowShouldClose(window->getGLFWWindow())) {
        glfwPollEvents();
        drawFrame();
    }
}

/**
 *     This function draws a single frame
 *
 */

 void Renderer::drawFrame() {
  NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
        CA::MetalDrawable *drawable = window->getMTLLayer()->nextDrawable();
        if (!drawable) {
            // * Clean up
            pool->release();
            glfwDestroyWindow(window->getGLFWWindow());
            return;
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

        // *  Encoding
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


Window &Renderer::getWindow() {
      return *window;
}

void Renderer::cameraUp() {
    std::cout << "Camera Up" << std::endl;
    camera.moveUp(0.05);
    // Before you draw a frame, update viewMatrix sent to the gpu
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;  // Copy updated view matrix to uniform buffer
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));

    drawFrame();
}

void Renderer::cameraDown() {
    camera.moveDown(0.05);
    // Before you draw a frame, update viewMatrix sent to the gpu
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;  // Copy updated view matrix to uniform buffer
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));

    drawFrame();

}

void Renderer::cameraRight() {
    camera.moveRight(0.05);
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));
    drawFrame();
}
void Renderer::cameraLeft() {
    camera.moveLeft(0.05);
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));
    drawFrame();
}

void Renderer::cameraZoom(float aZoom) {
    camera.zoom(aZoom);
    // ! TODO: abstract this repeated code for movement
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));
    drawFrame();
}

// ^ Static callbacks- Have to be static per GLFW's documentation.
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    auto *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
    if (!renderer || height == 0) return;


    renderer->getWindow().getMTLLayer()->setDrawableSize(CGSize{
        static_cast<double>(width),
        static_cast<double>(height)
    });

    float aspect = static_cast<float>(width) / height;
    renderer->updateProjectionMatrix(aspect);
    renderer->drawFrame();
}

void framebuffer_refresh_callback(GLFWwindow* window) {
    // ! THIS IS NOT TRIGGERING???
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // ! TODO, make this handle key holds, not just presses.
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    // ^ W
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        renderer->cameraUp();
    }
    // ^ A
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        renderer->cameraLeft();
    }
    // ^ S
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        renderer->cameraDown();
    }
    // ^ D
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        renderer->cameraRight();
    }
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    std::cout << "ScrollCallback" << std::endl;
    std::cout << "Scroll: x = " << xoffset << ", y = " << yoffset << std::endl;
    renderer->cameraZoom(yoffset);
};
